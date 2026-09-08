#include "qtcommsengine/Crc32.h"

namespace qtcommsengine
{

    quint32 Crc32::compute(const QByteArray &data)
    {
        quint32 crc = 0xFFFFFFFFu;

        for (int i = 0; i < data.size(); ++i)
        {
            crc ^= static_cast<quint8>(data.at(i));

            for (int bit = 0; bit < 8; ++bit)
            {
                if (crc & 1u)
                {
                    crc = (crc >> 1) ^ 0xEDB88320u;
                }
                else
                {
                    crc >>= 1;
                }
            }
        }

        return crc ^ 0xFFFFFFFFu;
    }

}
