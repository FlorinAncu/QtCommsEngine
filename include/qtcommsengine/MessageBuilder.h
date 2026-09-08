#pragma once

#include <QString>
#include <QByteArray>

#include "qtcommsengine/Message.h"
#include "qtcommsengine/Protocol.h"

namespace qtcommsengine
{

    // Factory for constructing the well-known protocol messages.
    class MessageBuilder
    {
    public:
        MessageBuilder() = default;

        static Message makePing(const QString &payload);
        static Message makeGetStatus(const QString &payload);
        static Message makeSetParameter(const QString &payload);
        static Message makeGetProtocolVersion();
        static Message makeHeartbeat();

        QByteArray build(const Message &msg) const;
    };

}
