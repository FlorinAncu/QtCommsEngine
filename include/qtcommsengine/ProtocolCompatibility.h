#pragma once

#include <QtGlobal>

namespace qtcommsengine
{

    // Checks whether a client and a server protocol version can interoperate.
    class ProtocolCompatibility
    {
    public:
        static bool isCompatible(quint32 serverVersion, quint32 clientVersion);
        static bool canFallback(quint32 serverVersion, quint32 clientVersion);
    };

}
