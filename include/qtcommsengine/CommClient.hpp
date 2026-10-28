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

    /// Runs communication requests on a dedicated worker thread.
    ///
    /// Use the client-specific connect(), disconnect(), sendAndHandle(),
    /// enqueue() and enqueueResponse() methods for channel operations; the
    /// inherited low-level sendMessage() and receiveMessage() methods execute
    /// on the calling thread and must not be called while the channel is moved.
    class CommClient : public CommManager
    {
        Q_OBJECT

    public:
        /// Creates a client and starts its worker thread.
        ///
        /// The channel is not owned by the client. It is moved to the worker
        /// thread while the client runs and returned to the creating thread on stop.
        explicit CommClient(CommChannel *channel,
                            const RetryPolicy &retryPolicy = RetryPolicy(),
                            int timeoutMs = 200,
                            QObject *parent = nullptr);

        /// Stops the worker and restores the channel to the creating thread.
        ~CommClient();

        /// Stops processing and waits for the worker thread to finish.
        /// When called from the worker thread, requests shutdown without waiting.
        void stop();

        /// Opens the channel on the worker thread and waits for the result.
        bool connect() override;

        /// Closes the channel on the worker thread and waits for completion.
        void disconnect() override;

        /// Sends a request and synchronously waits for a matching response.
        ErrorCode sendAndHandle(const Message &msg);

        /// Adds a request to the priority queue for asynchronous processing.
        /// Messages still queued when the client stops are not sent.
        void enqueue(const Message &msg);

        /// Queues a response to an unsolicited message for transmission.
        void enqueueResponse(const Message &msg);

    signals:
        /// Emitted for each received message, including unsolicited messages.
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
