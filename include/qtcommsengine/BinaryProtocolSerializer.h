#pragma once

#include <QByteArray>

#include "qtcommsengine/Message.h"

namespace qtcommsengine
{

    // Serializes and deserializes messages to/from the binary wire format (version + id + payload + CRC).
    class BinaryProtocolSerializer
    {
    public:
        static QByteArray serialize(const Message &msg);
        static Message deserialize(const QByteArray &data);
    };

}
