#pragma once

#include <QObject>
#include <QThread>

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

        ErrorCode sendAndHandle(const Message &msg);
        void enqueue(const Message &msg);

    signals:
        void messageReceived(const Message& msg);

    private:
        ErrorCode trySendOnce(const Message &msg);
        void runSender();
        bool reconnectAndResynchronize();

        ResponseHandler m_responseHandler;
        RetryPolicy m_retryPolicy;
        int m_timeoutMs;

        MessageQueue m_queue;
        QThread m_senderThread;
        bool m_running;
    };

}
