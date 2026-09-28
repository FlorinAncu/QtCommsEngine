#pragma once

#include <QString>
#include <QtGlobal>

struct ConnectionConfig
{
    QString serverAddress;   // IP or hostname
    quint16 serverPort;      // port TCP
    quint16 protocolVersion; // protocol version
};
