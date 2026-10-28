#pragma once

namespace qtcommsengine
{

    /// Scheduling priority used by MessageQueue (Critical is highest).
    enum class MessagePriority
    {
        Low,
        Normal,
        High,
        Critical
    };

}
