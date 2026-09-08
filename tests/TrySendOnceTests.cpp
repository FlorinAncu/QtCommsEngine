#include <catch2/catch_test_macros.hpp>

#include "qtcommsengine/CommClient.h"
#include "qtcommsengine/ErrorCode.h"
#include "qtcommsengine/MessageBuilder.h"
#include "qtcommsengine/MockChannel.h"

using namespace qtcommsengine;

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
