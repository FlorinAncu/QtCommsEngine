#include "qtcommsengine/Message.hpp"

namespace qtcommsengine
{

    Message::Message()
        : m_id(0)
        , m_priority(MessagePriority::Normal)
    {
    }

    Message::Message(qint32 id, const QByteArray &payload)
        : m_id(id)
        , m_payload(payload)
        , m_priority(MessagePriority::Normal)
    {
    }

    qint32 Message::getId() const
    {
        return m_id;
    }

    const QByteArray &Message::getPayload() const
    {
        return m_payload;
    }

    quint32 Message::getCorrelationId() const
    {
        return m_correlationId;
    }

    void Message::setCorrelationId(quint32 correlationId)
    {
        m_correlationId = correlationId;
    }

    void Message::setPriority(MessagePriority priority)
    {
        m_priority = priority;
    }

    MessagePriority Message::getPriority() const
    {
        return m_priority;
    }

}
