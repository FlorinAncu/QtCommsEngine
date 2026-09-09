# QtCommsEngine Tests

This directory contains the full Catch2-based test suite for QtCommsEngine.  
All tests are deterministic, run entirely in-memory, and validate the behavior of the
communication engine across message handling, protocol validation, queue semantics,
and automatic reconnection.

The test runner is implemented using `QCoreApplication` to ensure Qt event loop
compatibility during asynchronous client operations.


## Test Runner

The entry point for the test suite is:

#define CATCH_CONFIG_RUNNER
#include <catch2/catch_all.hpp>
#include <QCoreApplication>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    return Catch::Session().run(argc, argv);
}
This ensures that all Qt-based components (threads, timers, channels) operate correctly
during testing.

Test Coverage
1. MessageQueue Tests
File: MessageQueueTests.cpp

These tests verify:

Priority-based message ordering

FIFO behavior within the same priority

Correct behavior after queue closure

waitForClose() timeout and success conditions

Validated scenarios:

Critical > Normal > Low ordering

pop() returns false after close()

waitForClose() succeeds immediately if already closed

waitForClose() times out when the queue remains open

2. ProtocolValidator Tests
File: ProtocolValidatorTests.cpp

These tests validate:

Acceptance of known message IDs with payload

Acceptance of empty payloads for control messages

Rejection of empty payloads for non-control messages

Rejection of unknown message IDs

Detection of sentinel payloads (CRC_ERROR, VERSION_MISMATCH)

The validator ensures strict protocol correctness before messages enter the client pipeline.

3. CommClient Reconnection Tests
File: ReconnectionTests.cpp

These tests verify the automatic reconnection logic:

Client reconnects after channel disconnect

Protocol resynchronization (handshake) after reconnect

sendAndHandle() fails while disconnected

sendAndHandle() succeeds after auto-reconnect

The tests use MockChannel to simulate disconnects and reconnection timing.

4. CommClient Send/Handle Tests
File: TrySendOnceTests.cpp

These tests validate:

Successful handling of valid Ping messages

Correct protocol version handshake

Failure when the channel cannot be opened

Correct error propagation (ErrorCode::ChannelError)

These tests ensure that the synchronous sendAndHandle() API behaves correctly under
normal and failure conditions.

Running the Tests
From the project root:


ctest --test-dir build -C Debug
Or run the test binary directly:


./build/tests/QtCommsEngineTests
All tests are self-contained and require no external server or network connection.

Notes
All tests use MockChannel for deterministic behavior.

Timing-based reconnection tests rely on small sleep intervals (200–600 ms).

The test suite is designed to be fast: typically under 50 ms total runtime.

Adding new tests is straightforward: include the relevant QtCommsEngine headers and
use Catch2 macros.

Summary
The tests/ directory provides full coverage for:

Message queue semantics

Protocol validation

Client send/handle logic

Automatic reconnection and protocol resync

This suite ensures that QtCommsEngine behaves predictably and safely under all expected
conditions.