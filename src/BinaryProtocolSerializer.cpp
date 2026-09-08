#include "qtcommsengine/BinaryProtocolSerializer.h"

#include <QtGlobal>

#include "qtcommsengine/ProtocolVersion.h"
#include "qtcommsengine/Crc32.h"

namespace qtcommsengine
{

    static void appendUInt32(QByteArray &buffer, quint32 value)
    {
        buffer.append(static_cast<char>((value >> 0) & 0xFF));
        buffer.append(static_cast<char>((value >> 8) & 0xFF));
        buffer.append(static_cast<char>((value >> 16) & 0xFF));
        buffer.append(static_cast<char>((value >> 24) & 0xFF));
    }

    static quint32 readUInt32(const QByteArray &buffer, int offset)
    {
        if (offset + 4 > buffer.size())
        {
            return 0;
        }

        return  static_cast<quint8>(buffer.at(offset + 0))
            | (static_cast<quint8>(buffer.at(offset + 1)) << 8)
            | (static_cast<quint8>(buffer.at(offset + 2)) << 16)
            | (static_cast<quint8>(buffer.at(offset + 3)) << 24);
    }

    QByteArray BinaryProtocolSerializer::serialize(const Message &msg)
    {
        QByteArray frame;

        // Layout: [version (4)] [id (4)] [payloadSize (4)] [payload] [crc32 (4)]

        quint32 version = PROTOCOL_VERSION;
        qint32 id = msg.getId();
        quint32 payloadSize = static_cast<quint32>(msg.getPayload().size());

        appendUInt32(frame, version);
        appendUInt32(frame, static_cast<quint32>(id));
        appendUInt32(frame, payloadSize);

        frame.append(msg.getPayload());

        quint32 crc = Crc32::compute(frame);
        appendUInt32(frame, crc);

        return frame;
    }

    Message BinaryProtocolSerializer::deserialize(const QByteArray &data)
    {
        if (data.size() < 4 + 4 + 4 + 4)
        {
            return Message();
        }

        int offset = 0;

        quint32 version = readUInt32(data, offset);
        offset += 4;

        qint32 id = static_cast<qint32>(readUInt32(data, offset));
        offset += 4;

        quint32 payloadSize = readUInt32(data, offset);
        offset += 4;

        if (version != PROTOCOL_VERSION)
        {
            return Message();
        }

        if (data.size() < offset + static_cast<int>(payloadSize) + 4)
        {
            return Message();
        }

        QByteArray frameWithoutCrc = data.left(offset + static_cast<int>(payloadSize));
        quint32 expectedCrc = Crc32::compute(frameWithoutCrc);
        quint32 actualCrc = readUInt32(data, offset + static_cast<int>(payloadSize));

        if (expectedCrc != actualCrc)
        {
            return Message();
        }

        QByteArray payload = data.mid(offset, static_cast<int>(payloadSize));

        return Message(id, payload);
    }

}
