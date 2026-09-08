#pragma once

namespace qtcommsengine
{

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
