#include <catch2/catch_test_macros.hpp>
#include <QMutexLocker>
#include <QQueue>
#include <QSemaphore>
#include <QThread>

#include "qtcommsengine/BinaryProtocolSerializer.hpp"
#include "qtcommsengine/CommClient.hpp"
#include "qtcommsengine/ErrorCode.hpp"
#include "qtcommsengine/MessageBuilder.hpp"
#include "qtcommsengine/MessageParser.hpp"
#include "qtcommsengine/MockChannel.hpp"
#include "qtcommsengine/Protocol.hpp"

using namespace qtcommsengine;

class DuplexMockChannel : public MockChannel
{
public:
    void deliver(const Message &message)
    {
        deliverRaw(BinaryProtocolSerializer::serialize(message));
    }

    void deliverRaw(const QByteArray &data)
    {
        QMutexLocker locker(&mutex);
        incoming.enqueue(data);
    }

    QByteArray receive(int) override
    {
        QMutexLocker locker(&mutex);
        return incoming.isEmpty() ? QByteArray() : incoming.dequeue();
    }

    bool send(const QByteArray &data) override
    {
        if (injectRequestOnSend || injectOtherReplyOnSend)
        {
            const Message request = BinaryProtocolSerializer::deserialize(data);
            if (injectRequestOnSend)
            {
                Message incoming = MessageBuilder::makeGetStatus("server-status");
                incoming.setCorrelationId(0x80000001U);
                deliver(incoming);
            }
            if (injectOtherReplyOnSend)
            {
                Message other(static_cast<qint32>(MessageId::Pong), QByteArray("other"));
                other.setCorrelationId(request.getCorrelationId() + 1);
                deliver(other);
            }
            Message reply(static_cast<qint32>(MessageId::Pong), request.getPayload());
            reply.setCorrelationId(request.getCorrelationId());
            deliver(reply);
        }
        sent = data;
        sentSignal.release();
        return true;
    }

    bool injectRequestOnSend = false;
    bool injectOtherReplyOnSend = false;
    QSemaphore sentSignal;
    QByteArray sent;

private:
    QMutex mutex;
    QQueue<QByteArray> incoming;
};

class WaitingMockChannel : public MockChannel
{
public:
    void setTimeout(int milliseconds) override
    {
        timeoutMs = milliseconds;
    }

    QByteArray receive(int maxSize) override
    {
        QThread::msleep(static_cast<unsigned long>(timeoutMs));
        return MockChannel::receive(maxSize);
    }

    bool send(const QByteArray &data) override
    {
        const bool sent = MockChannel::send(data);
        sendSignal.release();
        return sent;
    }

    QSemaphore sendSignal;

private:
    int timeoutMs = 200;
};

class ThreadCheckingChannel : public MockChannel
{
public:
    bool open() override
    {
        checkThread();
        return MockChannel::open();
    }

    void close() override
    {
        checkThread();
        MockChannel::close();
    }

    bool send(const QByteArray &data) override
    {
        checkThread();
        return MockChannel::send(data);
    }

    QByteArray receive(int maxSize) override
    {
        checkThread();
        return MockChannel::receive(maxSize);
    }

    std::atomic_bool wrongThread = false;

private:
    void checkThread()
    {
        if (QThread::currentThread() != thread())
        {
            wrongThread = true;
        }
    }
};

TEST_CASE("CommClient synchronous operations use the channel thread", "[CommClient]")
{
    ThreadCheckingChannel channel;
    CommClient client(&channel);

    REQUIRE(client.connect());
    REQUIRE(client.sendAndHandle(MessageBuilder::makePing("sync")) == ErrorCode::Ok);
    client.disconnect();
    REQUIRE_FALSE(channel.wrongThread.load());
}

TEST_CASE("CommClient remains responsive while listening for unsolicited messages", "[CommClient]")
{
    WaitingMockChannel channel;
    CommClient client(&channel, RetryPolicy(), 1500);
    REQUIRE(client.connect());

    QThread::msleep(250);
    client.enqueue(MessageBuilder::makePing("responsive"));
    REQUIRE(channel.sendSignal.tryAcquire(1, 700));
}

TEST_CASE("CommClient sends requests with a QObject parent", "[CommClient]")
{
    QObject parent;
    WaitingMockChannel channel;
    CommClient client(&channel, RetryPolicy(), 200, &parent);
    REQUIRE(client.thread() == parent.thread());
    REQUIRE(client.connect());

    client.enqueue(MessageBuilder::makePing("parented"));
    REQUIRE(channel.sendSignal.tryAcquire(1, 1000));
}

TEST_CASE("MessageBuilder and MessageParser use the binary wire format", "[Protocol]")
{
    Message original = MessageBuilder::makePing("payload");
    original.setCorrelationId(42);
    const QByteArray frame = MessageBuilder().build(original);
    bool ok = false;
    const Message parsed = MessageParser().parse(frame, ok);

    REQUIRE(ok);
    REQUIRE(parsed.getId() == original.getId());
    REQUIRE(parsed.getPayload() == original.getPayload());
    REQUIRE(parsed.getCorrelationId() == original.getCorrelationId());

    QByteArray corrupted = frame;
    corrupted[16] = 'X';
    MessageParser().parse(corrupted, ok);
    REQUIRE_FALSE(ok);

    QByteArray oversized = frame;
    oversized[12] = static_cast<char>(0xFF);
    oversized[13] = static_cast<char>(0xFF);
    oversized[14] = static_cast<char>(0xFF);
    oversized[15] = static_cast<char>(0xFF);
    MessageParser().parse(oversized, ok);
    REQUIRE_FALSE(ok);

    MessageParser().parse(frame + 'X', ok);
    REQUIRE_FALSE(ok);
}

TEST_CASE("CommManager retains fragmented and coalesced frames", "[CommClient]")
{
    DuplexMockChannel channel;
    CommManager manager(&channel);
    REQUIRE(manager.connect());

    const QByteArray firstFrame = BinaryProtocolSerializer::serialize(MessageBuilder::makePing("first"));
    const QByteArray secondFrame = BinaryProtocolSerializer::serialize(MessageBuilder::makePing("second"));
    channel.deliverRaw(firstFrame.left(7));

    Message received;
    REQUIRE_FALSE(manager.receiveMessage(received));

    channel.deliverRaw(firstFrame.mid(7) + secondFrame);
    REQUIRE(manager.receiveMessage(received));
    REQUIRE(received.getPayload() == QByteArray("first"));
    REQUIRE(manager.receiveMessage(received));
    REQUIRE(received.getPayload() == QByteArray("second"));
}

TEST_CASE("CommClient replies to a server-initiated request", "[CommClient]")
{
    DuplexMockChannel channel;
    CommClient client(&channel);
    REQUIRE(client.connect());

    QObject::connect(&client, &CommClient::messageReceived, &client,
                     [&client](const Message &request)
                     {
                         Message response(static_cast<qint32>(MessageId::Pong), request.getPayload());
                         response.setCorrelationId(request.getCorrelationId());
                         client.enqueueResponse(response);
                     }, Qt::DirectConnection);

    Message request = MessageBuilder::makePing("from-server");
    request.setCorrelationId(0x80000001U);
    channel.deliver(request);

    REQUIRE(channel.sentSignal.tryAcquire(1, 3000));
    const Message response = BinaryProtocolSerializer::deserialize(channel.sent);
    REQUIRE(response.getId() == static_cast<qint32>(MessageId::Pong));
    REQUIRE(response.getPayload() == QByteArray("from-server"));
    REQUIRE(response.getCorrelationId() == request.getCorrelationId());
}

TEST_CASE("CommClient skips a reply with the wrong correlation ID", "[CommClient]")
{
    DuplexMockChannel channel;
    channel.injectOtherReplyOnSend = true;
    CommClient client(&channel);
    REQUIRE(client.connect());

    int received = 0;
    QObject::connect(&client, &CommClient::messageReceived, &client,
                     [&received](const Message &) { ++received; }, Qt::DirectConnection);

    REQUIRE(client.sendAndHandle(MessageBuilder::makePing("expected")) == ErrorCode::Ok);
    REQUIRE(received == 2);
}

TEST_CASE("CommClient waits for the matching reply across an unsolicited request", "[CommClient]")
{
    DuplexMockChannel channel;
    channel.injectRequestOnSend = true;
    CommClient client(&channel);
    REQUIRE(client.connect());

    QSemaphore requestSeen;
    QObject::connect(&client, &CommClient::messageReceived, &client,
                     [&requestSeen](const Message &message)
                     {
                         if (message.getId() == static_cast<qint32>(MessageId::GetStatus))
                         {
                             requestSeen.release();
                         }
                     }, Qt::DirectConnection);

    REQUIRE(client.sendAndHandle(MessageBuilder::makePing("expected")) == ErrorCode::Ok);
    REQUIRE(requestSeen.tryAcquire());
}

TEST_CASE("CommClient::sendAndHandle succeeds for valid ping", "[CommClient]")
{
    MockChannel channel;
    RetryPolicy retry{3, 200, 300};
    CommClient client(&channel, retry, 2000);

    REQUIRE(client.connect());
    REQUIRE(client.sendAndHandle(MessageBuilder::makePing("hi")) == ErrorCode::Ok);
}

TEST_CASE("CommClient validates protocol version handshake", "[CommClient]")
{
    MockChannel channel;
    RetryPolicy retry{3, 200, 300};
    CommClient client(&channel, retry, 2000);

    REQUIRE(client.connect());
    REQUIRE(client.sendAndHandle(MessageBuilder::makeGetProtocolVersion()) == ErrorCode::Ok);
}

TEST_CASE("CommClient fails when channel is not connected", "[CommClient]")
{
    MockChannel channel;
    channel.setOpenShouldFail(true);

    RetryPolicy retry{3, 200, 300};
    CommClient client(&channel, retry, 2000);

    // Channel cannot be opened → sendAndHandle must return ChannelError
    REQUIRE(client.sendAndHandle(MessageBuilder::makePing("hi")) == ErrorCode::ChannelError);
}
