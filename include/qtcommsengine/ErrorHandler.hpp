#pragma once

#include "qtcommsengine/ErrorCode.hpp"

namespace qtcommsengine
{

    /// Writes a human-readable description of an ErrorCode to Logger.
    class ErrorHandler
    {
    public:
        /// Logs the supplied result code.
        static void handle(ErrorCode code);
    };

}
