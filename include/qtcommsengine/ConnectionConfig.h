#pragma once

#include <QString>
#include <QtGlobal>

struct ConnectionConfig
{
    QString serverAddress;   // IP sau hostname
    quint16 serverPort;      // port TCP
    quint16 protocolVersion; // versiunea protocolului
};
