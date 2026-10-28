#pragma once

#include <QByteArray>
#include <QtGlobal>

namespace qtcommsengine
{

    /// Computes the CRC-32/ISO-HDLC checksum used by the wire protocol.
    class Crc32
    {
    public:
        /// Returns the checksum of data.
        static quint32 compute(const QByteArray &data);
    };

}
