#pragma once

#include <QObject>
#include <QList>
#include <QMutex>
#include <QWaitCondition>

#include "qtcommsengine/Message.h"

namespace qtcommsengine
{

    // Thread-safe priority queue used to hand messages from producers to CommClient's sender thread.
    class MessageQueue : public QObject
    {
        Q_OBJECT

    public:
        explicit MessageQueue(QObject *parent = nullptr);

        void push(const Message &msg);
        bool pop(Message &msg);
        bool pop(Message &msg, int timeoutMs);

        void close();
        bool waitForClose(int milliseconds);

    private:
        QMutex m_mutex;
        QWaitCondition m_condition;
        bool m_closed;

        QList<Message> m_queue;
    };

}
