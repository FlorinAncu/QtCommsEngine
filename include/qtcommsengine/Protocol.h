#pragma once

namespace qtcommsengine
{

    enum class MessageId : int
    {
        Ping = 1,
        Pong = 2,
        GetProtocolVersion = 7,
        ProtocolVersion = 8,
        Heartbeat = 9,
        GetStatus = 10,
        Status = 11,
        HeartbeatAck = 12,
        SetParameter = 20,
        ParameterAck = 21,
        Error = 99
    };

}
