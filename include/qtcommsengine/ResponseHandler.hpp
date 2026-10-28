#pragma once

#include "qtcommsengine/ErrorCode.hpp"
#include "qtcommsengine/Message.hpp"

namespace qtcommsengine
{

    /// Maps protocol responses to communication result codes.
    class ResponseHandler
    {
    public:
        /// Maps Error responses to ProtocolError and other responses to Ok.
        ErrorCode handleResponse(const Message &msg);
    };

}
