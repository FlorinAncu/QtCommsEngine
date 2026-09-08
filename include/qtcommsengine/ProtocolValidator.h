#pragma once

#include "qtcommsengine/ErrorCode.h"
#include "qtcommsengine/Message.h"

namespace qtcommsengine
{

    // Validates messages against the known protocol rules before sending or after receiving.
    class ProtocolValidator
    {
    public:
        static ErrorCode validate(const Message &msg);
    };

}
