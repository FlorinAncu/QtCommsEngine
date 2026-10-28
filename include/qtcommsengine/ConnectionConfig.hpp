#pragma once

#include <QString>
#include <QtGlobal>

struct ConnectionConfig
{
    /// IP address or hostname of the server; empty when not configured.
    QString serverAddress;

    /// TCP port; zero when not configured.
    quint16 serverPort = 0;

    /// Protocol version; zero when not configured.
    quint16 protocolVersion = 0;
};
