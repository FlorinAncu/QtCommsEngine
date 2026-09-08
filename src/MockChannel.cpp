#include "qtcommsengine/MockChannel.h"

namespace qtcommsengine
{

    MockChannel::MockChannel(QObject *parent)
        : CommChannel(parent)
        , m_connected(false)
        , m_timeoutMs(200)
    {
    }

    bool MockChannel::open()
    {
        QMutexLocker locker(&m_mutex);
        if (m_openShouldFail)
        {
            return false;
        }
        m_connected = true;
        return true;
    }

    void MockChannel::close()
    {
        QMutexLocker locker(&m_mutex);
        m_connected = false;
    }

    void MockChannel::setTimeout(int milliseconds)
    {
        m_timeoutMs = milliseconds;
        Q_UNUSED(m_timeoutMs);
    }

    bool MockChannel::send(const QByteArray &data)
    {
        QMutexLocker locker(&m_mutex);
        if (!m_connected)
        {
            return false;
        }

        m_responseData = data;
        return true;
    }

    QByteArray MockChannel::receive(int maxSize)
    {
        QMutexLocker locker(&m_mutex);
        if (!m_connected)
        {
            return QByteArray();
        }

        if (m_responseData.size() > maxSize)
        {
            return m_responseData.left(maxSize);
        }

        return m_responseData;
    }

    bool MockChannel::isConnected() const
    {
        QMutexLocker locker(&m_mutex);
        return m_connected;
    }
	
	void MockChannel::forceDisconnect()
	{
		{
			QMutexLocker locker(&m_mutex);
			m_connected = false;
		}
		emit disconnected();
	}

}
