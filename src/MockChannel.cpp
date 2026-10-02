#include "qtcommsengine/MockChannel.hpp"
#include "qtcommsengine/BinaryProtocolSerializer.hpp"
#include "qtcommsengine/Protocol.hpp"
#include "qtcommsengine/ProtocolVersion.hpp"

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

    void MockChannel::setOpenShouldFail(bool shouldFail)
    {
        QMutexLocker locker(&m_mutex);
        m_openShouldFail = shouldFail;
    }

    bool MockChannel::send(const QByteArray &data)
    {
        QMutexLocker locker(&m_mutex);
        if (!m_connected)
        {
            return false;
        }

        const Message request = BinaryProtocolSerializer::deserialize(data);
        QByteArray payload = request.getPayload();
        MessageId responseId = MessageId::Error;
        switch (static_cast<MessageId>(request.getId()))
        {
        case MessageId::Ping: responseId = MessageId::Pong; break;
        case MessageId::GetStatus: responseId = MessageId::Status; break;
        case MessageId::GetProtocolVersion:
            responseId = MessageId::ProtocolVersion;
            payload = QByteArray::number(PROTOCOL_VERSION);
            break;
        case MessageId::Heartbeat:
            responseId = MessageId::HeartbeatAck;
            payload.clear();
            break;
        case MessageId::SetParameter: responseId = MessageId::ParameterAck; break;
        default: payload = QByteArray("Unsupported request"); break;
        }
        Message response(static_cast<qint32>(responseId), payload);
        response.setCorrelationId(request.getCorrelationId());
        m_responseData = BinaryProtocolSerializer::serialize(response);
        return true;
    }

    QByteArray MockChannel::receive(int maxSize)
    {
        QMutexLocker locker(&m_mutex);
        if (!m_connected)
        {
            return QByteArray();
        }

        const QByteArray chunk = m_responseData.left(maxSize);
        m_responseData.remove(0, chunk.size());
        return chunk;
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
