#include "qtcommsengine/MessageParser.h"

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
        ok = false;

        if (rawData.isEmpty())
        {
            return Message();
        }

        qint32 id = static_cast<qint32>(static_cast<quint8>(rawData.at(0)));
        QByteArray payload = rawData.mid(1);

        ok = true;
        return Message(id, payload);
    }

}
