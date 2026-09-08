#include <catch2/catch_test_macros.hpp>

#include "qtcommsengine/ErrorCode.h"
#include "qtcommsengine/Message.h"
#include "qtcommsengine/MessageBuilder.h"
#include "qtcommsengine/ProtocolValidator.h"

using namespace qtcommsengine;

TEST_CASE("ProtocolValidator accepts known message IDs with payload", "[ProtocolValidator]")
{
    Message msg = MessageBuilder::makePing("hello");
    REQUIRE(ProtocolValidator::validate(msg) == ErrorCode::Ok);
}

TEST_CASE("ProtocolValidator accepts empty payload for control messages", "[ProtocolValidator]")
{
    REQUIRE(ProtocolValidator::validate(MessageBuilder::makeGetProtocolVersion()) == ErrorCode::Ok);
    REQUIRE(ProtocolValidator::validate(MessageBuilder::makeHeartbeat()) == ErrorCode::Ok);
}

TEST_CASE("ProtocolValidator rejects empty payload for non-control messages", "[ProtocolValidator]")
{
    Message msg(static_cast<int>(MessageId::Ping), QByteArray());
    REQUIRE(ProtocolValidator::validate(msg) == ErrorCode::InvalidMessage);
}

TEST_CASE("ProtocolValidator rejects unknown message IDs", "[ProtocolValidator]")
{
    Message msg(9999, QByteArray("x"));
    REQUIRE(ProtocolValidator::validate(msg) == ErrorCode::UnknownMessageId);
}

TEST_CASE("ProtocolValidator reports protocol errors from sentinel payloads", "[ProtocolValidator]")
{
    Message crcError(0, QByteArray("CRC_ERROR"));
    REQUIRE(ProtocolValidator::validate(crcError) == ErrorCode::ProtocolError);

    Message versionMismatch(0, QByteArray("VERSION_MISMATCH"));
    REQUIRE(ProtocolValidator::validate(versionMismatch) == ErrorCode::ProtocolError);
}
