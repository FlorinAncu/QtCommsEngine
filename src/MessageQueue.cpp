#include "qtcommsengine/MessageQueue.h"

#include <algorithm>

namespace qtcommsengine
{

    MessageQueue::MessageQueue(QObject *parent)
        : QObject(parent)
        , m_closed(false)
    {
    }

    void MessageQueue::push(const Message &msg)
    {
        QMutexLocker locker(&m_mutex);

        if (m_closed)
        {
            return;
        }

        m_queue.append(msg);

        std::sort(m_queue.begin(), m_queue.end(),
                  [](const Message &a, const Message &b)
                  {
                      return static_cast<int>(a.getPriority()) >
                             static_cast<int>(b.getPriority());
                  });

        m_condition.wakeOne();
    }

    bool MessageQueue::pop(Message &msg)
    {
        QMutexLocker locker(&m_mutex);

        while (!m_closed && m_queue.isEmpty())
        {
            m_condition.wait(&m_mutex);
        }

        if (m_queue.isEmpty())
        {
            return false;
        }

        msg = m_queue.front();
        m_queue.pop_front();
        return true;
    }

    bool MessageQueue::pop(Message &msg, int timeoutMs)
    {
        QMutexLocker locker(&m_mutex);

        while (!m_closed && m_queue.isEmpty())
        {
            if (!m_condition.wait(&m_mutex, timeoutMs))
            {
                return false; // timeout
            }
        }

        if (m_queue.isEmpty())
        {
            return false;
        }

        msg = m_queue.front();
        m_queue.pop_front();
        return true;
    }

    void MessageQueue::close()
    {
        QMutexLocker locker(&m_mutex);
        m_closed = true;
        m_condition.wakeAll();
    }

    bool MessageQueue::waitForClose(int milliseconds)
    {
        QMutexLocker locker(&m_mutex);

        // If already closed, return immediately
        if (m_closed)
        {
            return true;
        }

        // Wait until close() wakes us or timeout expires
        while (!m_closed)
        {
            if (!m_condition.wait(&m_mutex, milliseconds))
            {
                return false; // timeout
            }
        }

        return true;
    }

}
