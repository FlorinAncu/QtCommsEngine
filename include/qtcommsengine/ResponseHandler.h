#pragma once

#include "qtcommsengine/ErrorCode.h"
#include "qtcommsengine/Message.h"

namespace qtcommsengine
{

    // Interprets server responses and maps them to an ErrorCode.
    class ResponseHandler
    {
    public:
        ErrorCode handleResponse(const Message &msg);
    };

}
