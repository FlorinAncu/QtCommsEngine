#include <catch2/catch_test_macros.hpp>

#include "qtcommsengine/CommClient.h"
#include "qtcommsengine/ErrorCode.h"
#include "qtcommsengine/MessageBuilder.h"
#include "qtcommsengine/MockChannel.h"

using namespace qtcommsengine;

TEST_CASE("CommClient reconnects automatically after channel disconnect", "[CommClient][Reconnect]")
{
    MockChannel channel;
    RetryPolicy retry{3, 200, 300};
    CommClient client(&channel, retry, 2000);

    // 1. Initial connection
    REQUIRE(client.connect());
    REQUIRE(client.isConnected());

    // 2. Simulate connection loss
    channel.forceDisconnect();
    REQUIRE(!client.isConnected());

    // 3. Wait for auto-reconnect
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    REQUIRE(client.isConnected());

    // 4. After reconnect, normal messages should work
    REQUIRE(client.sendAndHandle(MessageBuilder::makePing("after-reconnect")) == ErrorCode::Ok);
}

TEST_CASE("CommClient performs protocol resync after reconnect", "[CommClient][Reconnect]")
{
    MockChannel channel;
    RetryPolicy retry{3, 200, 300};
    CommClient client(&channel, retry, 2000);

    REQUIRE(client.connect());
    REQUIRE(client.isConnected());

    // Simulate disconnect
    channel.forceDisconnect();
    REQUIRE(!client.isConnected());

    // Wait for auto-reconnect
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    REQUIRE(client.isConnected());

    // After reconnect, protocol handshake should succeed
    REQUIRE(client.sendAndHandle(MessageBuilder::makeGetProtocolVersion()) == ErrorCode::Ok);
    REQUIRE(client.sendAndHandle(MessageBuilder::makeHeartbeat()) == ErrorCode::Ok);
}

TEST_CASE("CommClient fails while disconnected but succeeds after auto-reconnect", "[CommClient][Reconnect]")
{
    MockChannel channel;
    RetryPolicy retry{3, 200, 300};
    CommClient client(&channel, retry, 2000);

    REQUIRE(client.connect());
    REQUIRE(client.isConnected());

    // Disconnect
    channel.forceDisconnect();
    REQUIRE(!client.isConnected());

    // Before reconnect → should fail
    REQUIRE(client.sendAndHandle(MessageBuilder::makePing("x")) == ErrorCode::ChannelError);

    // Wait for auto-reconnect
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    REQUIRE(client.isConnected());

    // After reconnect → should succeed
    REQUIRE(client.sendAndHandle(MessageBuilder::makePing("y")) == ErrorCode::Ok);
}
