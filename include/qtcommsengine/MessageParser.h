#pragma once

#include <QString>
#include <QByteArray>

#include "qtcommsengine/Message.h"
#include "qtcommsengine/Protocol.h"

namespace qtcommsengine
{

    // Extracts the id and payload of a Message and parses raw bytes back into one.
    class MessageParser
    {
    public:
        MessageParser() = default;

        static MessageId getId(const Message &msg);
        static QString getPayload(const Message &msg);

        Message parse(const QByteArray &rawData, bool &ok) const;
    };

}
