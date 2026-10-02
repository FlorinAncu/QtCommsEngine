#pragma once

#include <QObject>
#include <QMutex>
#include <QQueue>
#include <QSemaphore>
#include <QSharedPointer>
#include <QThread>
#include <atomic>

#include "qtcommsengine/CommManager.hpp"
#include "qtcommsengine/ErrorCode.hpp"
#include "qtcommsengine/MessageQueue.hpp"
#include "qtcommsengine/ResponseHandler.hpp"
#include "qtcommsengine/RetryPolicy.hpp"

namespace qtcommsengine
{

    class CommClient : public CommManager
    {
        Q_OBJECT

    public:
        explicit CommClient(CommChannel *channel,
                            const RetryPolicy &retryPolicy = RetryPolicy(),
                            int timeoutMs = 200,
                            QObject *parent = nullptr);

        ~CommClient();

        void stop();
        bool connect() override;
        void disconnect() override;
        ErrorCode sendAndHandle(const Message &msg);
        void enqueue(const Message &msg);
        void enqueueResponse(const Message &msg);

    signals:
        void messageReceived(const Message& msg);

    private:
        struct PendingCall
        {
            enum Kind { Connect, Disconnect, Send } kind;
            Message message;
            ErrorCode result = ErrorCode::ChannelError;
            QSemaphore completed;
        };

        ErrorCode invokeOnWorker(PendingCall::Kind kind, const Message &msg = {});
        ErrorCode trySendOnce(const Message &msg);
        void runSender();
        bool reconnectAndResynchronize();

        ResponseHandler m_responseHandler;
        RetryPolicy m_retryPolicy;
        int m_timeoutMs;

        MessageQueue m_queue;
        QMutex m_ioMutex;
        QMutex m_callsMutex;
        QQueue<QSharedPointer<PendingCall>> m_pendingCalls;
        QMutex m_responseMutex;
        QQueue<Message> m_pendingResponses;
        QThread m_senderThread;
        QThread *m_ownerThread;
        quint32 m_nextCorrelationId = 0;
        std::atomic_bool m_running;
    };

}
