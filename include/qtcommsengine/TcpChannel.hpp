#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QtGlobal>

#include "qtcommsengine/CommChannel.hpp"

namespace qtcommsengine
{

    /// TCP implementation of CommChannel backed by QTcpSocket.
    class TcpChannel : public CommChannel
    {
        Q_OBJECT

    public:
        /// Creates a TCP channel for the specified host and port.
        TcpChannel(const QString &host, quint16 port, QObject *parent = nullptr);
        ~TcpChannel() override;

        /// Connects to the configured endpoint using the current timeout.
        bool open() override;

        /// Closes the socket.
        void close() override;
        
        /// Changes the host used by the next connection attempt.
        void setHost(const QString& host);

        /// Changes the port used by the next connection attempt.
        void setPort(quint16 port);

        /// Sets the connect, send and receive timeout in milliseconds.
        void setTimeout(int milliseconds) override;

        /// Writes data to the connected socket.
        bool send(const QByteArray &data) override;

        /// Reads up to maxSize bytes, waiting up to the configured timeout.
        QByteArray receive(int maxSize) override;

        /// Returns whether the socket is connected.
        bool isConnected() const override;

    private:
        QString m_host;
        quint16 m_port;
        QTcpSocket *m_socket;
        int m_timeoutMs;
    };

}
