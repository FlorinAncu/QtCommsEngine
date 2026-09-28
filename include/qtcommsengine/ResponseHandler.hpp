#pragma once

#include "qtcommsengine/ErrorCode.hpp"
#include "qtcommsengine/Message.hpp"

namespace qtcommsengine
{

    // Interprets server responses and maps them to an ErrorCode.
    class ResponseHandler
    {
    public:
        ErrorCode handleResponse(const Message &msg);
    };

}
