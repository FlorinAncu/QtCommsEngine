#include "qtcommsengine/ProtocolCompatibility.h"

namespace qtcommsengine
{

    bool ProtocolCompatibility::isCompatible(quint32 serverVersion, quint32 clientVersion)
    {
        return serverVersion == clientVersion;
    }

    bool ProtocolCompatibility::canFallback(quint32 serverVersion, quint32 clientVersion)
    {
        Q_UNUSED(serverVersion);
        Q_UNUSED(clientVersion);
        return false;
    }

}
