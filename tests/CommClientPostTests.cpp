#include <catch2/catch_test_macros.hpp>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QHostAddress>
#include <QTcpServer>
#include <QThread>

#include "qtcommsengine/CommClient.hpp"
#include "qtcommsengine/TcpChannel.hpp"
#include "qtcommsengine/TcpServer.hpp"

using namespace qtcommsengine;

namespace
{
    template <typename Predicate>
    bool waitUntil(Predicate predicate, int timeoutMs)
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
}

TEST_CASE("CommClient posts one-way messages without waiting for a response", "[CommClient][post]")
{
    const quint16 port = findAvailablePort();
    REQUIRE(port != 0);

    TcpServer server(QStringLiteral("127.0.0.1"), port);
    REQUIRE(server.listen());

    int connections = 0;
    int disconnections = 0;
    QList<QByteArray> received;
    QObject::connect(&server, &TcpServer::clientConnected, &server,
                     [&connections](TcpServer::ClientId) { ++connections; });
    QObject::connect(&server, &TcpServer::clientDisconnected, &server,
                     [&disconnections](TcpServer::ClientId) { ++disconnections; });
    QObject::connect(&server, &TcpServer::messageReceived, &server,
                     [&received](TcpServer::ClientId, const Message &message)
                     {
                         received.append(message.getPayload());
                     });

    const int responseTimeoutMs = 100;
    TcpChannel channel(QStringLiteral("127.0.0.1"), port);
    CommClient client(&channel, RetryPolicy(), responseTimeoutMs);
    REQUIRE(client.connect());
    REQUIRE(waitUntil([&connections]() { return connections == 1; }, 1000));

    // Application message IDs have no defined response; enqueue() would time out.
    client.post(Message(1000, QByteArray("first")));
    client.post(Message(1000, QByteArray("second")));
    client.post(Message(1001, QByteArray("third")));

    REQUIRE(waitUntil([&received]() { return received.size() == 3; }, 2000));
    REQUIRE(received == QList<QByteArray>{"first", "second", "third"});

    // Several response timeouts later the connection must still be the original one.
    waitUntil([]() { return false; }, responseTimeoutMs * 6);
    REQUIRE(connections == 1);
    REQUIRE(disconnections == 0);

    client.stop();
}
