#include <catch2/catch_test_macros.hpp>
#include <QCoreApplication>
#include <QEventLoop>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#include "qtcommsengine/TcpServer.hpp"

namespace
{
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

        if (!server.hasClient())
        {
            eventLoop.exec();
        }

        return server.hasClient();
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