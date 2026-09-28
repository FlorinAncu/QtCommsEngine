#pragma once

#include "qtcommsengine/ErrorCode.hpp"
#include "qtcommsengine/Message.hpp"

namespace qtcommsengine
{

    // Validates messages against the known protocol rules before sending or after receiving.
    class ProtocolValidator
    {
    public:
        static ErrorCode validate(const Message &msg);
    };

}
