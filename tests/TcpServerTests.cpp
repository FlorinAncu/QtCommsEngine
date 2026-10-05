#include <catch2/catch_test_macros.hpp>
#include <QCoreApplication>
#include <QEventLoop>
#include <QHostAddress>
#include <QElapsedTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

#include "qtcommsengine/BinaryProtocolSerializer.hpp"
#include "qtcommsengine/TcpServer.hpp"

namespace
{
    template <typename Predicate>
    bool waitUntil(Predicate predicate, int timeoutMs = 1000)
    {
        QElapsedTimer timer;
        timer.start();
        while (!predicate() && timer.elapsed() < timeoutMs)
        {
            QCoreApplication::processEvents();
            QThread::msleep(1);
        }
        QCoreApplication::processEvents();
        return predicate();
    }

    quint16 findAvailablePort()
    {
        QTcpServer probe;
        if (!probe.listen(QHostAddress(QStringLiteral("127.0.0.1")), 0))
        {
            return 0;
        }

        const quint16 port = probe.serverPort();
        probe.close();
        return port;
    }

    bool acceptsClient(qtcommsengine::TcpServer &server,
                       const QHostAddress &address,
                       quint16 port)
    {
        QTcpSocket client;
        client.connectToHost(address, port);
        if (!client.waitForConnected(1000))
        {
            return false;
        }

        QEventLoop eventLoop;
        QTimer timeout;
        timeout.setSingleShot(true);
        QObject::connect(&server, &qtcommsengine::TcpServer::clientConnected,
                         &eventLoop, &QEventLoop::quit);
        QObject::connect(&timeout, &QTimer::timeout,
                         &eventLoop, &QEventLoop::quit);
        timeout.start(1000);

        if (!server.hasClients())
        {
            eventLoop.exec();
        }

        return server.hasClients();
    }
}

TEST_CASE("TcpServer wildcard accepts IPv4 loopback clients", "[TcpServer][IPv4]")
{
    const quint16 port = findAvailablePort();
    REQUIRE(port != 0);

    qtcommsengine::TcpServer server(QStringLiteral("*"), port);
    REQUIRE(server.listen());
    REQUIRE(acceptsClient(server, QHostAddress(QStringLiteral("127.0.0.1")), port));
}

TEST_CASE("TcpServer wildcard accepts IPv6 loopback clients", "[TcpServer][IPv6]")
{
    QTcpServer ipv6Probe;
    if (!ipv6Probe.listen(QHostAddress::LocalHostIPv6, 0))
    {
        SUCCEED("IPv6 loopback is unavailable in this environment");
        return;
    }
    ipv6Probe.close();

    const quint16 port = findAvailablePort();
    REQUIRE(port != 0);

    qtcommsengine::TcpServer server(QStringLiteral("*"), port);
    REQUIRE(server.listen());
    REQUIRE(acceptsClient(server, QHostAddress::LocalHostIPv6, port));
}

TEST_CASE("TcpServer accepts multiple clients and routes messages per client", "[TcpServer][multi-client]")
{
    const quint16 port = findAvailablePort();
    REQUIRE(port != 0);

    qtcommsengine::TcpServer server(QStringLiteral("127.0.0.1"), port);
    REQUIRE(server.listen());

    QList<qtcommsengine::TcpServer::ClientId> connectedIds;
    QObject::connect(&server, &qtcommsengine::TcpServer::clientConnected,
                     &server, [&connectedIds](qtcommsengine::TcpServer::ClientId clientId)
                     {
                         connectedIds.append(clientId);
                     });

    QTcpSocket firstClient;
    QTcpSocket secondClient;
    firstClient.connectToHost(QHostAddress::LocalHost, port);
    REQUIRE(firstClient.waitForConnected(1000));
    REQUIRE(waitUntil([&server]() { return server.clientCount() == 1; }));
    secondClient.connectToHost(QHostAddress::LocalHost, port);
    REQUIRE(secondClient.waitForConnected(1000));
    REQUIRE(waitUntil([&server]() { return server.clientCount() == 2; }));
    REQUIRE(server.hasClients());

    REQUIRE(connectedIds.size() == 2);
    REQUIRE(connectedIds[0] != connectedIds[1]);
    REQUIRE_FALSE(server.send(qtcommsengine::Message(2, QByteArray("ambiguous"))));

    QList<qtcommsengine::TcpServer::ClientId> receivedClientIds;
    QList<QByteArray> receivedPayloads;
    QObject::connect(&server, &qtcommsengine::TcpServer::messageReceived,
                     &server,
                     [&receivedClientIds, &receivedPayloads](
                         qtcommsengine::TcpServer::ClientId clientId,
                         const qtcommsengine::Message &message)
                     {
                         receivedClientIds.append(clientId);
                         receivedPayloads.append(message.getPayload());
                     });

    const QByteArray request = qtcommsengine::BinaryProtocolSerializer::serialize(
        qtcommsengine::Message(1, QByteArray("first")));
    const qsizetype partialSize = request.size() / 2;
    REQUIRE(firstClient.write(request.left(partialSize)) == partialSize);
    REQUIRE(secondClient.write(request) == request.size());
    REQUIRE(waitUntil([&receivedClientIds]() { return receivedClientIds.size() == 1; }));
    REQUIRE(receivedClientIds[0] == connectedIds[1]);
    REQUIRE(receivedPayloads[0] == QByteArray("first"));
    REQUIRE(firstClient.write(request.mid(partialSize)) == request.size() - partialSize);
    REQUIRE(waitUntil([&receivedClientIds]() { return receivedClientIds.size() == 2; }));
    REQUIRE(receivedClientIds[1] == connectedIds[0]);
    REQUIRE(receivedPayloads[1] == QByteArray("first"));

    const qtcommsengine::Message response(3, QByteArray("only-first"));
    REQUIRE(server.sendToClient(connectedIds[0], response));
    REQUIRE(waitUntil([&firstClient]() { return firstClient.bytesAvailable() > 0; }));
    const auto decodedResponse = qtcommsengine::BinaryProtocolSerializer::deserialize(
        firstClient.readAll());
    REQUIRE(decodedResponse.getPayload() == QByteArray("only-first"));
    REQUIRE(secondClient.bytesAvailable() == 0);

    REQUIRE(server.broadcast(qtcommsengine::Message(4, QByteArray("all"))));
    REQUIRE(waitUntil([&firstClient, &secondClient]() {
        return firstClient.bytesAvailable() > 0 && secondClient.bytesAvailable() > 0;
    }));
    REQUIRE(qtcommsengine::BinaryProtocolSerializer::deserialize(firstClient.readAll()).getPayload()
            == QByteArray("all"));
    REQUIRE(qtcommsengine::BinaryProtocolSerializer::deserialize(secondClient.readAll()).getPayload()
            == QByteArray("all"));
}

TEST_CASE("TcpServer enforces the configured maximum client count", "[TcpServer][client-limit]")
{
    const quint16 port = findAvailablePort();
    REQUIRE(port != 0);

    qtcommsengine::TcpServer server(QStringLiteral("127.0.0.1"), port);
    REQUIRE(server.setMaximumClients(1));
    REQUIRE(server.maximumClients() == 1);
    REQUIRE_FALSE(server.setMaximumClients(-1));
    REQUIRE(server.listen());

    QTcpSocket acceptedClient;
    acceptedClient.connectToHost(QHostAddress::LocalHost, port);
    REQUIRE(acceptedClient.waitForConnected(1000));
    REQUIRE(waitUntil([&server]() { return server.clientCount() == 1; }));

    QTcpSocket rejectedClient;
    rejectedClient.connectToHost(QHostAddress::LocalHost, port);
    REQUIRE(rejectedClient.waitForConnected(1000));
    REQUIRE(waitUntil([&rejectedClient]() {
        return rejectedClient.state() == QAbstractSocket::UnconnectedState;
    }));
    REQUIRE(server.clientCount() == 1);
    REQUIRE(server.hasClients());

    acceptedClient.disconnectFromHost();
    REQUIRE(waitUntil([&server]() { return server.clientCount() == 0; }));

    QTcpSocket replacementClient;
    replacementClient.connectToHost(QHostAddress::LocalHost, port);
    REQUIRE(replacementClient.waitForConnected(1000));
    REQUIRE(waitUntil([&server]() { return server.clientCount() == 1; }));
}

TEST_CASE("TcpServer send methods have explicit behavior with no or one client", "[TcpServer][send]")
{
    const quint16 port = findAvailablePort();
    REQUIRE(port != 0);

    qtcommsengine::TcpServer server(QStringLiteral("127.0.0.1"), port);
    const qtcommsengine::Message message(5, QByteArray("single"));
    REQUIRE_FALSE(server.send(message));
    REQUIRE_FALSE(server.sendToClient(1, message));
    REQUIRE_FALSE(server.broadcast(message));
    REQUIRE(server.listen());

    QTcpSocket client;
    client.connectToHost(QHostAddress::LocalHost, port);
    REQUIRE(client.waitForConnected(1000));
    REQUIRE(waitUntil([&server]() { return server.clientCount() == 1; }));

    const auto clientId = server.clientIds().value(0);
    REQUIRE(clientId != 0);
    REQUIRE_FALSE(server.sendToClient(clientId + 1, message));
    REQUIRE(server.send(message));
    REQUIRE(waitUntil([&client]() { return client.bytesAvailable() > 0; }));
    REQUIRE(qtcommsengine::BinaryProtocolSerializer::deserialize(client.readAll()).getPayload()
            == QByteArray("single"));

    client.disconnectFromHost();
    REQUIRE(waitUntil([&server]() { return server.clientCount() == 0; }));
    REQUIRE_FALSE(server.send(message));
    REQUIRE_FALSE(server.sendToClient(clientId, message));
    REQUIRE_FALSE(server.broadcast(message));
}

TEST_CASE("TcpServer zero limit is unlimited and lowering a limit preserves clients",
          "[TcpServer][client-limit]")
{
    const quint16 port = findAvailablePort();
    REQUIRE(port != 0);

    qtcommsengine::TcpServer server(QStringLiteral("127.0.0.1"), port);
    REQUIRE(server.setMaximumClients(0));
    REQUIRE(server.maximumClients() == 0);
    REQUIRE(server.listen());

    QTcpSocket firstClient;
    QTcpSocket secondClient;
    firstClient.connectToHost(QHostAddress::LocalHost, port);
    REQUIRE(firstClient.waitForConnected(1000));
    REQUIRE(waitUntil([&server]() { return server.clientCount() == 1; }));
    secondClient.connectToHost(QHostAddress::LocalHost, port);
    REQUIRE(secondClient.waitForConnected(1000));
    REQUIRE(waitUntil([&server]() { return server.clientCount() == 2; }));

    REQUIRE(server.setMaximumClients(1));
    REQUIRE(server.clientCount() == 2);

    QTcpSocket excessClient;
    excessClient.connectToHost(QHostAddress::LocalHost, port);
    REQUIRE(excessClient.waitForConnected(1000));
    REQUIRE(waitUntil([&excessClient]() {
        return excessClient.state() == QAbstractSocket::UnconnectedState;
    }));
    REQUIRE(server.clientCount() == 2);
}

TEST_CASE("TcpServer close disconnects every client and reports each client ID",
          "[TcpServer][lifecycle]")
{
    const quint16 port = findAvailablePort();
    REQUIRE(port != 0);

    qtcommsengine::TcpServer server(QStringLiteral("127.0.0.1"), port);
    REQUIRE(server.listen());

    QList<qtcommsengine::TcpServer::ClientId> disconnectedIds;
    QObject::connect(&server, &qtcommsengine::TcpServer::clientDisconnected,
                     &server, [&disconnectedIds](qtcommsengine::TcpServer::ClientId clientId)
                     {
                         disconnectedIds.append(clientId);
                     });

    QTcpSocket firstClient;
    QTcpSocket secondClient;
    firstClient.connectToHost(QHostAddress::LocalHost, port);
    REQUIRE(firstClient.waitForConnected(1000));
    REQUIRE(waitUntil([&server]() { return server.clientCount() == 1; }));
    secondClient.connectToHost(QHostAddress::LocalHost, port);
    REQUIRE(secondClient.waitForConnected(1000));
    REQUIRE(waitUntil([&server]() { return server.clientCount() == 2; }));

    const auto connectedIds = server.clientIds();
    REQUIRE(connectedIds.size() == 2);
    server.close();

    REQUIRE_FALSE(server.isListening());
    REQUIRE_FALSE(server.hasClients());
    REQUIRE(server.clientCount() == 0);
    REQUIRE(disconnectedIds.size() == 2);
    REQUIRE(disconnectedIds.contains(connectedIds[0]));
    REQUIRE(disconnectedIds.contains(connectedIds[1]));
    REQUIRE(waitUntil([&firstClient]() {
        return firstClient.state() == QAbstractSocket::UnconnectedState;
    }));
    REQUIRE(waitUntil([&secondClient]() {
        return secondClient.state() == QAbstractSocket::UnconnectedState;
    }));
}