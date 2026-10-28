#pragma once

#include <QString>
#include <QByteArray>

#include "qtcommsengine/Message.hpp"
#include "qtcommsengine/Protocol.hpp"

namespace qtcommsengine
{

    /// Convenience accessors and binary-frame parser for Message values.
    class MessageParser
    {
    public:
        MessageParser() = default;

        /// Converts the message's numeric ID to the corresponding protocol enum.
        static MessageId getId(const Message &msg);

        /// Decodes the message payload as UTF-8 text.
        static QString getPayload(const Message &msg);

        /// Parses exactly one complete frame and sets ok to indicate success.
        Message parse(const QByteArray &rawData, bool &ok) const;
    };

}
