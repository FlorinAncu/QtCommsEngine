#pragma once

#include <QByteArray>

#include "qtcommsengine/Message.hpp"

namespace qtcommsengine
{

    /// Encodes and decodes versioned binary message frames.
    ///
    /// Frames contain little-endian version, message ID, correlation ID and
    /// payload-size fields, followed by the payload and a CRC32 checksum.
    class BinaryProtocolSerializer
    {
    public:
        /// Serializes a message into one complete wire-format frame.
        static QByteArray serialize(const Message &msg);

        /// Deserializes one complete frame. Returns a default Message if it is invalid.
        static Message deserialize(const QByteArray &data);
    };

}
