#include "qtcommsengine/CommManager.h"
#include "qtcommsengine/BinaryProtocolSerializer.h"

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

        return m_channel->open();
    }

    void CommManager::disconnect()
    {
        if (m_channel)
        {
            m_channel->close();
        }
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

        QByteArray frame = BinaryProtocolSerializer::serialize(msg);
        return m_channel->send(frame);
    }

    bool CommManager::receiveMessage(Message &msg)
    {
        if (!isConnected())
        {
            return false;
        }

        QByteArray raw = m_channel->receive(4096);

        if (raw.isEmpty())
        {
            return false;
        }

        msg = BinaryProtocolSerializer::deserialize(raw);
        return true;
    }

    CommChannel *CommManager::transport() const
    {
        return m_channel;
    }

}
