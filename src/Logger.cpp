#include "qtcommsengine/Logger.h"

#include <QTextStream>

namespace qtcommsengine
{

    static QTextStream &out()
    {
        static QTextStream ts(stdout);
        return ts;
    }

    void Logger::info(const QString &msg)
    {
        out() << "[INFO] " << msg << Qt::endl;
    }

    void Logger::warn(const QString &msg)
    {
        out() << "[WARN] " << msg << Qt::endl;
    }

    void Logger::error(const QString &msg)
    {
        out() << "[ERROR] " << msg << Qt::endl;
    }

}
