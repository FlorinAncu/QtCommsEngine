#pragma once

namespace qtcommsengine
{

    /// Result codes returned by communication and protocol operations.
    enum class ErrorCode
    {
        Ok = 0,
        ChannelError,
        Timeout,
        InvalidMessage,
        UnknownMessageId,
        ProtocolError
    };

}
