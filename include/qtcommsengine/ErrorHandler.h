#pragma once

#include "qtcommsengine/ErrorCode.h"

namespace qtcommsengine
{

    // Logs ErrorCode values produced by the communication layer.
    class ErrorHandler
    {
    public:
        static void handle(ErrorCode code);
    };

}
