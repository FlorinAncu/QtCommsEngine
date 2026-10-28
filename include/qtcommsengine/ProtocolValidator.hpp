#pragma once

#include "qtcommsengine/ErrorCode.hpp"
#include "qtcommsengine/Message.hpp"

namespace qtcommsengine
{

    /// Validates message IDs and payload rules when explicitly invoked.
    class ProtocolValidator
    {
    public:
        /// Returns Ok for a valid message, otherwise the relevant validation error.
        static ErrorCode validate(const Message &msg);
    };

}
