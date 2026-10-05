#pragma once

#include <QByteArray>
#include <QHash>
#include <QList>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "qtcommsengine/Message.hpp"

class QTcpServer;
class QTcpSocket;

namespace qtcommsengine
{

    /// Asynchronous TCP server that keeps an independent protocol session per client.
    class TcpServer : public QObject
    {
        Q_OBJECT

    public:
        using ClientId = quint64;

        TcpServer(const QString &host, quint16 port, QObject *parent = nullptr);

        bool listen();
        void close();
        bool isListening() const;

        /// Returns whether at least one client is connected.
        bool hasClients() const;

        /// Returns the number of currently connected clients.
        int clientCount() const;

        /// Returns the IDs of connected clients; the order is unspecified.
        QList<ClientId> clientIds() const;

        /// Returns the active-client limit; zero means unlimited.
        int maximumClients() const;

        /// Sets the active-client limit. Zero means unlimited; negative values are rejected.
        /// Lowering the limit does not disconnect clients that are already connected.
        bool setMaximumClients(int maximumClients);

        /// Sends only when exactly one client is connected; fails if none or multiple are connected.
        bool send(const Message &message);

        /// Sends to the specified connected client.
        bool sendToClient(ClientId clientId, const Message &message);

        /// Sends to every connected client and returns true only if every write succeeds.
        bool broadcast(const Message &message);
        void setHost(const QString &host);
        void setPort(quint16 port);

    signals:
        /// Emitted for each valid message, together with the ID of its sender.
        void messageReceived(qtcommsengine::TcpServer::ClientId clientId,
                             const qtcommsengine::Message &message);

        /// Emitted when a client connects, with its server-assigned ID.
        void clientConnected(qtcommsengine::TcpServer::ClientId clientId);

        /// Emitted when a client disconnects or is removed by close().
        void clientDisconnected(qtcommsengine::TcpServer::ClientId clientId);

        void protocolError(const QString &message);

    private slots:
        void acceptPendingConnection();

    private:
        struct ClientSession
        {
            QTcpSocket *socket = nullptr;
            QByteArray receiveBuffer;
        };

        void readClientData(QTcpSocket *client);
        void handleClientDisconnected(QTcpSocket *client);
        bool writeToClient(QTcpSocket *client, const Message &message);

        QString m_host;
        quint16 m_port;
        QTcpServer *m_server;
        QTcpServer *m_ipv4Server;
        QHash<ClientId, ClientSession> m_clients;
        QHash<QTcpSocket *, ClientId> m_clientIds;
        ClientId m_nextClientId = 0;
        int m_maximumClients = 0;
    };

}