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
        , m_client(nullptr)
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
        if (m_client)
        {
            m_client->disconnectFromHost();
            m_client->deleteLater();
            m_client = nullptr;
        }

        m_receiveBuffer.clear();
        m_server->close();
        m_ipv4Server->close();
    }

    bool TcpServer::isListening() const
    {
        return m_server->isListening() || m_ipv4Server->isListening();
    }

    bool TcpServer::hasClient() const
    {
        return m_client && m_client->state() == QAbstractSocket::ConnectedState;
    }

    bool TcpServer::send(const Message &message)
    {
        if (!hasClient())
        {
            return false;
        }

        const QByteArray frame = BinaryProtocolSerializer::serialize(message);
        const qint64 bytesQueued = m_client->write(frame);
        m_client->flush();
        return bytesQueued == frame.size();
    }

    void TcpServer::acceptPendingConnection()
    {
        const auto acceptPendingFrom = [this](QTcpServer *server)
        {
            while (server->hasPendingConnections())
            {
                QTcpSocket *pendingClient = server->nextPendingConnection();

                if (hasClient())
                {
                    pendingClient->disconnectFromHost();
                    pendingClient->deleteLater();
                    continue;
                }

                m_client = pendingClient;
                m_receiveBuffer.clear();

                connect(m_client, &QTcpSocket::readyRead,
                        this, &TcpServer::readClientData);
                connect(m_client, &QTcpSocket::disconnected,
                        this, &TcpServer::handleClientDisconnected);

                emit clientConnected();
            }
        };

        acceptPendingFrom(m_server);
        acceptPendingFrom(m_ipv4Server);
    }

    void TcpServer::readClientData()
    {
        if (!m_client)
        {
            return;
        }

        m_receiveBuffer.append(m_client->readAll());

        while (m_receiveBuffer.size() >= headerSize)
        {
            const quint32 payloadSize = readUInt32(m_receiveBuffer, 12);
            if (payloadSize > maximumPayloadSize)
            {
                emit protocolError(QStringLiteral("Incoming payload exceeds the size limit"));
                m_receiveBuffer.clear();
                m_client->disconnectFromHost();
                return;
            }

            const qsizetype frameSize = headerSize + static_cast<qsizetype>(payloadSize) + crcSize;
            if (m_receiveBuffer.size() < frameSize)
            {
                return;
            }

            const QByteArray frame = m_receiveBuffer.left(frameSize);
            m_receiveBuffer.remove(0, frameSize);

            const Message message = BinaryProtocolSerializer::deserialize(frame);
            if (message.getId() == 0)
            {
                emit protocolError(QStringLiteral("Invalid protocol frame received"));
                continue;
            }

            emit messageReceived(message);
        }
    }

    void TcpServer::handleClientDisconnected()
    {
        if (!m_client)
        {
            return;
        }

        m_client->deleteLater();
        m_client = nullptr;
        m_receiveBuffer.clear();
        emit clientDisconnected();
    }

}