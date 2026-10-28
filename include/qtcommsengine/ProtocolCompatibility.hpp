#pragma once

#include <QtGlobal>

namespace qtcommsengine
{

    /// Compatibility checks for protocol version numbers.
    class ProtocolCompatibility
    {
    public:
        /// Returns true only when the versions are identical.
        static bool isCompatible(quint32 serverVersion, quint32 clientVersion);

        /// Returns whether the implementation supports fallback between versions.
        static bool canFallback(quint32 serverVersion, quint32 clientVersion);
    };

}
