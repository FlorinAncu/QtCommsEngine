#include "qtcommsengine/MessageBuilder.hpp"
#include "qtcommsengine/BinaryProtocolSerializer.hpp"

namespace qtcommsengine
{

    Message MessageBuilder::makePing(const QString &payload)
    {
        return Message(static_cast<qint32>(MessageId::Ping),
                       payload.toUtf8());
    }

    Message MessageBuilder::makeGetStatus(const QString &payload)
    {
        return Message(static_cast<qint32>(MessageId::GetStatus),
                       payload.toUtf8());
    }

    Message MessageBuilder::makeSetParameter(const QString &payload)
    {
        return Message(static_cast<qint32>(MessageId::SetParameter),
                       payload.toUtf8());
    }

    Message MessageBuilder::makeGetProtocolVersion()
    {
        return Message(static_cast<qint32>(MessageId::GetProtocolVersion),
                       QByteArray());
    }

    Message MessageBuilder::makeHeartbeat()
    {
        return Message(static_cast<qint32>(MessageId::Heartbeat),
                       QByteArray());
    }

    QByteArray MessageBuilder::build(const Message &msg) const
    {
        return BinaryProtocolSerializer::serialize(msg);
    }

}
