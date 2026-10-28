#pragma once

#include <QString>

namespace qtcommsengine
{

    /// Minimal process-wide logger that writes messages to standard output.
    class Logger
    {
    public:
        /// Writes an informational message.
        static void info(const QString &msg);

        /// Writes a warning message.
        static void warn(const QString &msg);

        /// Writes an error message.
        static void error(const QString &msg);
    };

}
