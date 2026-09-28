#include "qtcommsengine/CommClient.hpp"
#include "qtcommsengine/ErrorCode.hpp"
#include "qtcommsengine/MessageBuilder.hpp"
#include "qtcommsengine/MockChannel.hpp"

#include <QCoreApplication>
#include <QDebug>

// Minimal example: connect a CommClient to a MockChannel, send a ping and read the response.
int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    using namespace qtcommsengine;

    MockChannel channel;
    RetryPolicy retryPolicy{3, 500, 1000};
    CommClient client(&channel, retryPolicy, 2000);

    if (!client.connect())
    {
        qWarning() << "Failed to connect.";
        return 1;
    }

    Message ping = MessageBuilder::makePing("Hello, QtCommsEngine!");
    ErrorCode result = client.sendAndHandle(ping);

    qDebug() << "sendAndHandle result:" << static_cast<int>(result);

    return result == ErrorCode::Ok ? 0 : 1;
}
