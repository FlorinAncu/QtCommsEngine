#include <catch2/catch_test_macros.hpp>

#include "qtcommsengine/Message.h"
#include "qtcommsengine/MessagePriority.h"
#include "qtcommsengine/MessageQueue.h"

using namespace qtcommsengine;

TEST_CASE("MessageQueue delivers messages in priority order", "[MessageQueue]")
{
    MessageQueue queue;

    Message low(1, QByteArray());
    low.setPriority(MessagePriority::Low);

    Message critical(2, QByteArray());
    critical.setPriority(MessagePriority::Critical);

    Message normal(3, QByteArray());
    normal.setPriority(MessagePriority::Normal);

    queue.push(low);
    queue.push(critical);
    queue.push(normal);

    Message out;

    REQUIRE(queue.pop(out));
    REQUIRE(out.getId() == 2);

    REQUIRE(queue.pop(out));
    REQUIRE(out.getId() == 3);

    REQUIRE(queue.pop(out));
    REQUIRE(out.getId() == 1);
}

TEST_CASE("MessageQueue::pop returns false after close with no pending messages", "[MessageQueue]")
{
    MessageQueue queue;
    queue.close();

    Message out;
    REQUIRE_FALSE(queue.pop(out));
}

TEST_CASE("MessageQueue::waitForClose returns true once closed", "[MessageQueue]")
{
    MessageQueue queue;
    queue.close();

    REQUIRE(queue.waitForClose(50));
}

TEST_CASE("MessageQueue::waitForClose times out while open", "[MessageQueue]")
{
    MessageQueue queue;
    REQUIRE_FALSE(queue.waitForClose(20));
}
