#pragma once

#include <QString>
#include <QByteArray>

#include "qtcommsengine/Message.hpp"
#include "qtcommsengine/Protocol.hpp"

namespace qtcommsengine
{

    /// Factory functions for the protocol's commonly used request messages.
    class MessageBuilder
    {
    public:
        MessageBuilder() = default;

        /// Creates a Ping request carrying a UTF-8 encoded payload.
        static Message makePing(const QString &payload);

        /// Creates a GetStatus request carrying a UTF-8 encoded payload.
        static Message makeGetStatus(const QString &payload);

        /// Creates a SetParameter request carrying a UTF-8 encoded payload.
        static Message makeSetParameter(const QString &payload);

        /// Creates a GetProtocolVersion request with an empty payload.
        static Message makeGetProtocolVersion();

        /// Creates a Heartbeat request with an empty payload.
        static Message makeHeartbeat();

        /// Encodes msg as a complete binary protocol frame.
        QByteArray build(const Message &msg) const;
    };

}
