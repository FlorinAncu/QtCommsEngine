#include "qtcommsengine/MessageParser.hpp"
#include "qtcommsengine/BinaryProtocolSerializer.hpp"

namespace qtcommsengine
{

    MessageId MessageParser::getId(const Message &msg)
    {
        return static_cast<MessageId>(msg.getId());
    }

    QString MessageParser::getPayload(const Message &msg)
    {
        return QString::fromUtf8(msg.getPayload());
    }

    Message MessageParser::parse(const QByteArray &rawData, bool &ok) const
    {
        const Message message = BinaryProtocolSerializer::deserialize(rawData);
        ok = message.getId() != 0;
        return message;
    }

}
