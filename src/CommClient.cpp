#include "qtcommsengine/CommClient.hpp"
#include "qtcommsengine/Protocol.hpp"
#include <QElapsedTimer>
#include <QMutexLocker>

namespace qtcommsengine
{

    CommClient::CommClient(CommChannel *channel,
                           const RetryPolicy &retryPolicy,
                           int timeoutMs,
                           QObject *parent)
        : CommManager(channel, parent)
        , m_retryPolicy(retryPolicy)
        , m_timeoutMs(timeoutMs)
        , m_queue()
        , m_ownerThread(QThread::currentThread())
        , m_running(true)
    {
        setTimeout(timeoutMs);
        m_queue.moveToThread(&m_senderThread);
        if (transport())
        {
            transport()->moveToThread(&m_senderThread);
        }

        QObject::connect(&m_senderThread, &QThread::started,
                [this]() { runSender(); });

        m_senderThread.start();
    }

    CommClient::~CommClient()
    {
        stop();
    }

    void CommClient::stop()
    {
        m_running = false;
        m_queue.close();
        m_senderThread.quit();
        m_senderThread.wait();
    }

    void CommClient::enqueue(const Message &msg)
    {
        m_queue.push(msg);
    }

    void CommClient::enqueueResponse(const Message &msg)
    {
        QMutexLocker locker(&m_responseMutex);
        m_pendingResponses.enqueue(msg);
    }

    ErrorCode CommClient::invokeOnWorker(PendingCall::Kind kind, const Message &msg)
    {
        QSharedPointer<PendingCall> call(new PendingCall);
        call->kind = kind;
        call->message = msg;
        {
            QMutexLocker locker(&m_callsMutex);
            if (!m_running)
            {
                return ErrorCode::ChannelError;
            }
            m_pendingCalls.enqueue(call);
        }
        call->completed.acquire();
        return call->result;
    }

    bool CommClient::connect()
    {
        if (QThread::currentThread() == &m_senderThread)
        {
            return CommManager::connect();
        }
        return invokeOnWorker(PendingCall::Connect) == ErrorCode::Ok;
    }

    void CommClient::disconnect()
    {
        if (QThread::currentThread() == &m_senderThread)
        {
            CommManager::disconnect();
            return;
        }
        invokeOnWorker(PendingCall::Disconnect);
    }

    ErrorCode CommClient::sendAndHandle(const Message &msg)
    {
        if (QThread::currentThread() == &m_senderThread)
        {
            return trySendOnce(msg);
        }
        return invokeOnWorker(PendingCall::Send, msg);
    }

    ErrorCode CommClient::trySendOnce(const Message &msg)
    {
        QMutexLocker locker(&m_ioMutex);
        if (!isConnected())
        {
            return ErrorCode::ChannelError;
        }

        Message request = msg;
        if (request.getCorrelationId() == 0)
        {
            m_nextCorrelationId = (m_nextCorrelationId % 0x7fffffffU) + 1;
            request.setCorrelationId(m_nextCorrelationId);
        }

        if (!sendMessage(request))
        {
            return ErrorCode::ChannelError;
        }

        MessageId expectedId = MessageId::Error;
        switch (static_cast<MessageId>(msg.getId()))
        {
        case MessageId::Ping: expectedId = MessageId::Pong; break;
        case MessageId::GetStatus: expectedId = MessageId::Status; break;
        case MessageId::GetProtocolVersion: expectedId = MessageId::ProtocolVersion; break;
        case MessageId::Heartbeat: expectedId = MessageId::HeartbeatAck; break;
        case MessageId::SetParameter: expectedId = MessageId::ParameterAck; break;
        default: break;
        }

        QElapsedTimer timer;
        timer.start();
        while (m_running && timer.elapsed() < m_timeoutMs)
        {
            Message response;
            if (!receiveMessage(response, m_timeoutMs - static_cast<int>(timer.elapsed())))
            {
                continue;
            }

            emit messageReceived(response);
            if (response.getCorrelationId() == request.getCorrelationId()
                && (static_cast<MessageId>(response.getId()) == MessageId::Error
                    || static_cast<MessageId>(response.getId()) == expectedId))
            {
                return m_responseHandler.handleResponse(response);
            }
        }

        return ErrorCode::Timeout;
    }

    void CommClient::runSender()
    {
        while (m_running)
        {
            QSharedPointer<PendingCall> call;
            {
                QMutexLocker locker(&m_callsMutex);
                if (!m_pendingCalls.isEmpty())
                {
                    call = m_pendingCalls.dequeue();
                }
            }

            if (call)
            {
                if (call->kind == PendingCall::Send)
                {
                    call->result = trySendOnce(call->message);
                }
                else
                {
                    QMutexLocker locker(&m_ioMutex);
                    if (call->kind == PendingCall::Connect)
                    {
                        call->result = CommManager::connect() ? ErrorCode::Ok : ErrorCode::ChannelError;
                    }
                    else
                    {
                        CommManager::disconnect();
                        call->result = ErrorCode::Ok;
                    }
                }
                call->completed.release();
                continue;
            }

            Message response;
            bool hasResponse = false;
            {
                QMutexLocker locker(&m_responseMutex);
                if (!m_pendingResponses.isEmpty())
                {
                    response = m_pendingResponses.dequeue();
                    hasResponse = true;
                }
            }

            if (hasResponse)
            {
                QMutexLocker locker(&m_ioMutex);
                sendMessage(response);
                continue;
            }

            Message msg;
            if (!m_queue.pop(msg, 100))
            {
                if (!m_running)
                {
                    break;
                }

                if (!isConnected())
                {
                    reconnectAndResynchronize();
                }
                else
                {
                    QMutexLocker locker(&m_ioMutex);
                    Message incoming;
                    if (receiveMessage(incoming, 100) && incoming.getId() != 0)
                    {
                        emit messageReceived(incoming);
                    }
                }
                continue;
            }

            if (!m_running)
            {
                break;
            }

            ErrorCode code = trySendOnce(msg);

            if (code != ErrorCode::Ok)
            {
                reconnectAndResynchronize();
            }
        }

        {
            QMutexLocker locker(&m_callsMutex);
            while (!m_pendingCalls.isEmpty())
            {
                QSharedPointer<PendingCall> call = m_pendingCalls.dequeue();
                call->completed.release();
            }
        }
        CommManager::disconnect();
        if (transport())
        {
            transport()->moveToThread(m_ownerThread);
        }
        m_queue.moveToThread(m_ownerThread);
    }

    bool CommClient::reconnectAndResynchronize()
    {
        QMutexLocker locker(&m_ioMutex);
        CommManager::disconnect();
        QThread::msleep(m_retryPolicy.reconnectDelayMs);
        for (int attempt = 0; m_running && attempt < m_retryPolicy.maxRetries; ++attempt)
        {
            if (CommManager::connect())
            {
                return true;
            }

            if (attempt + 1 < m_retryPolicy.maxRetries)
            {
                QThread::msleep(m_retryPolicy.retryDelayMs);
            }
        }
        return false;
    }

}
