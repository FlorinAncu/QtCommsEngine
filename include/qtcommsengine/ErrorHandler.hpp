#pragma once

#include "qtcommsengine/ErrorCode.hpp"

namespace qtcommsengine
{

    // Logs ErrorCode values produced by the communication layer.
    class ErrorHandler
    {
    public:
        static void handle(ErrorCode code);
    };

}
