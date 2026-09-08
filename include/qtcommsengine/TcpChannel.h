#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QtGlobal>

#include "qtcommsengine/CommChannel.h"

namespace qtcommsengine
{

    // TCP/IP implementation of CommChannel using BSD sockets / Winsock.
    class TcpChannel : public CommChannel
    {
        Q_OBJECT

    public:
        TcpChannel(const QString &host, quint16 port, QObject *parent = nullptr);
        ~TcpChannel() override;

        bool open() override;
        void close() override;

        void setTimeout(int milliseconds) override;

        bool send(const QByteArray &data) override;
        QByteArray receive(int maxSize) override;

        bool isConnected() const override;

    private:
        QString m_host;
        quint16 m_port;
        QTcpSocket *m_socket;
        bool m_connected;
        int m_timeoutMs;
    };

}
