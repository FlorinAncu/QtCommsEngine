#include "qtcommsengine/TcpServer.hpp"

#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>

#include "qtcommsengine/BinaryProtocolSerializer.hpp"

namespace qtcommsengine
{

    namespace
    {
        constexpr int headerSize = 16;
        constexpr int crcSize = 4;
        constexpr quint32 maximumPayloadSize = 16U * 1024U * 1024U;

        quint32 readUInt32(const QByteArray &buffer, int offset)
        {
            return static_cast<quint8>(buffer.at(offset))
                | (static_cast<quint8>(buffer.at(offset + 1)) << 8)
                | (static_cast<quint8>(buffer.at(offset + 2)) << 16)
                | (static_cast<quint8>(buffer.at(offset + 3)) << 24);
        }
    }

    TcpServer::TcpServer(const QString &host, quint16 port, QObject *parent)
        : QObject(parent)
        , m_host(host)
        , m_port(port)
        , m_server(new QTcpServer(this))
        , m_ipv4Server(new QTcpServer(this))
    {
        connect(m_server, &QTcpServer::newConnection,
                this, &TcpServer::acceptPendingConnection);
        connect(m_ipv4Server, &QTcpServer::newConnection,
                this, &TcpServer::acceptPendingConnection);
    }

    void TcpServer::setHost(const QString &host)
    {
        m_host = host;
    }
    
    void TcpServer::setPort(quint16 port)
    {
        m_port = port;
    }

    bool TcpServer::listen()
    {
        const bool wildcard = m_host.isEmpty()
            || m_host == QStringLiteral("*")
            || m_host == QStringLiteral("0.0.0.0");
        if (wildcard)
        {
            if (m_server->isListening() || m_ipv4Server->isListening())
            {
                return true;
            }

            const bool ipv6Listening = m_server->listen(QHostAddress::AnyIPv6, m_port);
            const quint16 ipv4Port = ipv6Listening ? m_server->serverPort() : m_port;
            const bool ipv4Listening = m_ipv4Server->listen(QHostAddress::Any, ipv4Port);
            return ipv6Listening || ipv4Listening;
        }

        if (m_server->isListening())
        {
            return true;
        }

        QHostAddress address;
        if (!address.setAddress(m_host))
        {
            emit protocolError(QStringLiteral("Invalid listen address: %1").arg(m_host));
            return false;
        }

        return m_server->listen(address, m_port);
    }

    void TcpServer::close()
    {
        const auto sessions = m_clients;
        m_clients.clear();
        m_clientIds.clear();

        for (auto it = sessions.cbegin(); it != sessions.cend(); ++it)
        {
            QTcpSocket *client = it->socket;
            QObject::disconnect(client, nullptr, this, nullptr);
            client->abort();
            client->deleteLater();
            emit clientDisconnected(it.key());
        }

        m_server->close();
        m_ipv4Server->close();
    }

    bool TcpServer::isListening() const
    {
        return m_server->isListening() || m_ipv4Server->isListening();
    }

    bool TcpServer::hasClients() const
    {
        return !m_clients.isEmpty();
    }

    int TcpServer::clientCount() const
    {
        return m_clients.size();
    }

    QList<TcpServer::ClientId> TcpServer::clientIds() const
    {
        return m_clients.keys();
    }

    int TcpServer::maximumClients() const
    {
        return m_maximumClients;
    }

    bool TcpServer::setMaximumClients(int maximumClients)
    {
        if (maximumClients < 0)
        {
            return false;
        }

        m_maximumClients = maximumClients;
        return true;
    }

    bool TcpServer::send(const Message &message)
    {
        if (m_clients.size() != 1)
        {
            return false;
        }

        return writeToClient(m_clients.cbegin()->socket, message);
    }

    bool TcpServer::sendToClient(ClientId clientId, const Message &message)
    {
        const auto it = m_clients.constFind(clientId);
        if (it == m_clients.cend())
        {
            return false;
        }

        return writeToClient(it->socket, message);
    }

    bool TcpServer::broadcast(const Message &message)
    {
        if (m_clients.isEmpty())
        {
            return false;
        }

        bool allSent = true;
        for (auto it = m_clients.cbegin(); it != m_clients.cend(); ++it)
        {
            if (!writeToClient(it->socket, message))
            {
                allSent = false;
            }
        }

        return allSent;
    }

    bool TcpServer::writeToClient(QTcpSocket *client, const Message &message)
    {
        if (!client || client->state() != QAbstractSocket::ConnectedState)
        {
            return false;
        }

        const QByteArray frame = BinaryProtocolSerializer::serialize(message);
        const qint64 bytesQueued = client->write(frame);
        client->flush();
        return bytesQueued == frame.size();
    }

    void TcpServer::acceptPendingConnection()
    {
        const auto acceptPendingFrom = [this](QTcpServer *server)
        {
            while (server->hasPendingConnections())
            {
                QTcpSocket *pendingClient = server->nextPendingConnection();

                if (m_maximumClients > 0 && m_clients.size() >= m_maximumClients)
                {
                    pendingClient->abort();
                    pendingClient->deleteLater();
                    continue;
                }

                do
                {
                    ++m_nextClientId;
                } while (m_nextClientId == 0 || m_clients.contains(m_nextClientId));

                const ClientId clientId = m_nextClientId;
                m_clients.insert(clientId, ClientSession{pendingClient, {}});
                m_clientIds.insert(pendingClient, clientId);

                connect(pendingClient, &QTcpSocket::readyRead, this,
                        [this, pendingClient]() { readClientData(pendingClient); });
                connect(pendingClient, &QTcpSocket::disconnected, this,
                        [this, pendingClient]() { handleClientDisconnected(pendingClient); });

                emit clientConnected(clientId);
            }
        };

        acceptPendingFrom(m_server);
        acceptPendingFrom(m_ipv4Server);
    }

    void TcpServer::readClientData(QTcpSocket *client)
    {
        const auto idIt = m_clientIds.constFind(client);
        if (idIt == m_clientIds.cend())
        {
            return;
        }
        const ClientId clientId = idIt.value();

        m_clients[clientId].receiveBuffer.append(client->readAll());

        while (m_clients.contains(clientId))
        {
            QByteArray &receiveBuffer = m_clients[clientId].receiveBuffer;
            if (receiveBuffer.size() < headerSize)
            {
                return;
            }

            const quint32 payloadSize = readUInt32(receiveBuffer, 12);
            if (payloadSize > maximumPayloadSize)
            {
                emit protocolError(QStringLiteral("Incoming payload exceeds the size limit"));
                if (m_clients.contains(clientId))
                {
                    m_clients[clientId].receiveBuffer.clear();
                    client->disconnectFromHost();
                }
                return;
            }

            const qsizetype frameSize = headerSize + static_cast<qsizetype>(payloadSize) + crcSize;
            if (receiveBuffer.size() < frameSize)
            {
                return;
            }

            const QByteArray frame = receiveBuffer.left(frameSize);
            receiveBuffer.remove(0, frameSize);

            const Message message = BinaryProtocolSerializer::deserialize(frame);
            if (message.getId() == 0)
            {
                emit protocolError(QStringLiteral("Invalid protocol frame received"));
                continue;
            }

            emit messageReceived(clientId, message);
        }
    }

    void TcpServer::handleClientDisconnected(QTcpSocket *client)
    {
        const auto idIt = m_clientIds.find(client);
        if (idIt == m_clientIds.end())
        {
            return;
        }

        const ClientId clientId = idIt.value();
        m_clientIds.erase(idIt);
        m_clients.remove(clientId);
        client->deleteLater();
        emit clientDisconnected(clientId);
    }

}