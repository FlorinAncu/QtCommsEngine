#include "qtcommsengine/ProtocolValidator.h"
#include "qtcommsengine/Protocol.h"

namespace qtcommsengine
{

    ErrorCode ProtocolValidator::validate(const Message &msg)
    {
        // Sentinel payloads carry protocol-level errors regardless of message ID
        const QByteArray payload = msg.getPayload();
        if (payload == QByteArray("CRC_ERROR") || payload == QByteArray("VERSION_MISMATCH"))
        {
            return ErrorCode::ProtocolError;
        }

        MessageId id = static_cast<MessageId>(msg.getId());

        switch (id)
        {
        case MessageId::Ping:
        case MessageId::Pong:
        case MessageId::GetStatus:
        case MessageId::Status:
        case MessageId::SetParameter:
        case MessageId::ParameterAck:
        case MessageId::Error:
            // Non-control messages require a non-empty payload
            if (payload.isEmpty())
            {
                return ErrorCode::InvalidMessage;
            }
            return ErrorCode::Ok;

        case MessageId::GetProtocolVersion:
        case MessageId::ProtocolVersion:
        case MessageId::Heartbeat:
        case MessageId::HeartbeatAck:
            // Control messages may have an empty payload
            return ErrorCode::Ok;

        default:
            return ErrorCode::UnknownMessageId;
        }
    }

}
