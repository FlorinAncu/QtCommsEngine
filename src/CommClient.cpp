#include "qtcommsengine/CommClient.hpp"

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
        , m_running(true)
    {
        m_queue.moveToThread(&m_senderThread);
        if (transport())
        {
            transport()->moveToThread(&m_senderThread);
        }
        this->moveToThread(&m_senderThread);

        QObject::connect(&m_senderThread, &QThread::started,
                this, &CommClient::runSender);

        m_senderThread.start();
    }

    CommClient::~CommClient()
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

    ErrorCode CommClient::sendAndHandle(const Message &msg)
    {
        return trySendOnce(msg);
    }

    ErrorCode CommClient::trySendOnce(const Message &msg)
    {
        if (!isConnected())
        {
            return ErrorCode::ChannelError;
        }

        if (!sendMessage(msg))
        {
            return ErrorCode::ChannelError;
        }

        Message response;
        if (!receiveMessage(response))
        {
            return ErrorCode::Timeout;
        }

        ErrorCode code = m_responseHandler.handleResponse(response);

        if (code == ErrorCode::Ok)
        {
            emit messageReceived(response);
        }

        return code;
    }

    void CommClient::runSender()
    {
        while (m_running)
        {
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
                continue;
            }

            ErrorCode code = sendAndHandle(msg);

            if (code != ErrorCode::Ok)
            {
                reconnectAndResynchronize();
            }
        }
    }

    bool CommClient::reconnectAndResynchronize()
    {
        disconnect();
        QThread::msleep(m_retryPolicy.reconnectDelayMs);
        return connect();
    }

}
