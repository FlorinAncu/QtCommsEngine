#pragma once

#include <QObject>
#include <QThread>

#include "qtcommsengine/CommManager.h"
#include "qtcommsengine/ErrorCode.h"
#include "qtcommsengine/MessageQueue.h"
#include "qtcommsengine/ResponseHandler.h"
#include "qtcommsengine/RetryPolicy.h"

namespace qtcommsengine
{

    // Asynchronous client built on CommManager, adding a background sender queue, retries and auto-reconnect.
    class CommClient : public CommManager
    {
        Q_OBJECT

    public:
        explicit CommClient(CommChannel *channel,
                            const RetryPolicy &retryPolicy = RetryPolicy(),
                            int timeoutMs = 200,
                            QObject *parent = nullptr);

        ~CommClient();

        // Sends a message and processes the server response.
        ErrorCode sendAndHandle(const Message &msg);

        void enqueue(const Message &msg);

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
