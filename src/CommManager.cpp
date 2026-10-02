#include "qtcommsengine/CommManager.hpp"
#include "qtcommsengine/BinaryProtocolSerializer.hpp"
#include <QElapsedTimer>

namespace qtcommsengine
{

    CommManager::CommManager(CommChannel *channel, QObject *parent)
        : QObject(parent)
        , m_channel(channel)
        , m_timeoutMs(200)
    {
    }

    bool CommManager::connect()
    {
        if (!m_channel)
        {
            return false;
        }

        m_channel->setTimeout(m_timeoutMs);
        return m_channel->open();
    }

    void CommManager::disconnect()
    {
        if (m_channel)
        {
            m_channel->close();
        }
        m_receiveBuffer.clear();
    }

    void CommManager::setTimeout(int milliseconds)
    {
        m_timeoutMs = milliseconds;

        if (m_channel)
        {
            m_channel->setTimeout(milliseconds);
        }
    }

    bool CommManager::isConnected() const
    {
        return m_channel && m_channel->isConnected();
    }

    bool CommManager::sendMessage(const Message &msg)
    {
        if (!isConnected())
        {
            return false;
        }

        m_channel->setTimeout(m_timeoutMs);
        QByteArray frame = BinaryProtocolSerializer::serialize(msg);
        return m_channel->send(frame);
    }

    bool CommManager::receiveMessage(Message &msg, int timeoutMs)
    {
        if (!isConnected())
        {
            return false;
        }

        const int deadlineMs = timeoutMs >= 0 ? timeoutMs : m_timeoutMs;
        QElapsedTimer timer;
        timer.start();

        while (true)
        {
            if (m_receiveBuffer.size() >= 16)
            {
                const quint32 payloadSize = static_cast<quint8>(m_receiveBuffer.at(12))
                    | (static_cast<quint8>(m_receiveBuffer.at(13)) << 8)
                    | (static_cast<quint8>(m_receiveBuffer.at(14)) << 16)
                    | (static_cast<quint8>(m_receiveBuffer.at(15)) << 24);

                if (payloadSize > 16U * 1024U * 1024U)
                {
                    m_receiveBuffer.clear();
                    return false;
                }

                const qsizetype frameSize = 16 + static_cast<qsizetype>(payloadSize) + 4;
                if (m_receiveBuffer.size() >= frameSize)
                {
                    const QByteArray frame = m_receiveBuffer.left(frameSize);
                    m_receiveBuffer.remove(0, frameSize);
                    msg = BinaryProtocolSerializer::deserialize(frame);
                    return msg.getId() != 0;
                }
            }

            const int remainingMs = deadlineMs - static_cast<int>(timer.elapsed());
            if (remainingMs <= 0)
            {
                return false;
            }

            m_channel->setTimeout(remainingMs);
            const QByteArray chunk = m_channel->receive(4096);
            if (chunk.isEmpty())
            {
                return false;
            }
            m_receiveBuffer.append(chunk);
        }
    }

    CommChannel *CommManager::transport() const
    {
        return m_channel;
    }

}
