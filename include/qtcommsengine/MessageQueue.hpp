#pragma once

#include <QObject>
#include <QList>
#include <QMutex>
#include <QWaitCondition>

#include "qtcommsengine/Message.hpp"

namespace qtcommsengine
{

    /// Thread-safe priority queue for passing messages to a consumer thread.
    class MessageQueue : public QObject
    {
        Q_OBJECT

    public:
        /// Creates an open, empty queue.
        explicit MessageQueue(QObject *parent = nullptr);

        /// Adds a message; higher-priority messages are returned first.
        /// Messages with equal priority retain their insertion order.
        void push(const Message &msg);

        /// Waits until a message is available or the queue is closed.
        bool pop(Message &msg);

        /// Waits up to timeoutMs for a message, or until the queue is closed.
        bool pop(Message &msg, int timeoutMs);

        /// Closes the queue and wakes all waiting consumers.
        void close();

        /// Waits up to milliseconds for close(); returns false on timeout.
        bool waitForClose(int milliseconds);

    private:
        QMutex m_mutex;
        QWaitCondition m_condition;
        bool m_closed;

        QList<Message> m_queue;
    };

}
