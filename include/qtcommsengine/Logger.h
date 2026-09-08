#pragma once

#include <QString>

namespace qtcommsengine
{

    // Minimal static logger that writes info/warn/error messages to stdout.
    class Logger
    {
    public:
        static void info(const QString &msg);
        static void warn(const QString &msg);
        static void error(const QString &msg);
    };

}
