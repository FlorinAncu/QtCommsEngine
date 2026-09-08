#pragma once

#include <QByteArray>
#include <QtGlobal>

namespace qtcommsengine
{

    // Computes CRC32 checksums used to detect corrupted frames on the wire.
    class Crc32
    {
    public:
        static quint32 compute(const QByteArray &data);
    };

}
