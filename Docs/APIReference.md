# API Reference

QtCommsEngine is a C++20 static library built on Qt 6 Core and Network. Public
headers are under `include/qtcommsengine/`; most public types are in the
`qtcommsengine` namespace.

## Transport

- `CommChannel` defines the transport interface for opening, closing, sending
  and receiving raw bytes.
- `TcpChannel` implements that interface with `QTcpSocket`.
- `MockChannel` provides an in-memory implementation for tests and examples.
- `TcpServer` accepts multiple clients and exposes per-client routing through
  `ClientId`, `sendToClient()` and `broadcast()`.

Channels are QObject instances. Keep a channel alive for as long as its manager
uses it. `CommClient` moves its channel to a dedicated worker thread while it is
running and returns it to the creating thread when stopped; the channel must
therefore have no QObject parent.

## Runtime

`CommManager` provides blocking connect, disconnect, send and receive operations.
It does not own its channel, and direct use must follow the channel's thread
affinity.

`CommClient` serializes channel operations on its worker thread. Its synchronous
`connect()`, `disconnect()` and `sendAndHandle()` methods wait for work on that
thread. `enqueue()` schedules a request on the priority queue, while
`enqueueResponse()` queues a response to an unsolicited message.
`messageReceived` is emitted for incoming messages. On connection loss the
client attempts reconnection using `RetryPolicy`; it does not automatically
replay a failed request, generate periodic heartbeats or negotiate protocol
versions. Do not call the inherited low-level `sendMessage()` or
`receiveMessage()` methods while the client is running: they execute on the
calling thread rather than submitting work to the client thread.

## Messages and framing

`Message` stores a numeric ID, binary payload, correlation ID and priority.
`MessageBuilder` creates common request types, and `MessageParser` provides
convenience decoding for one complete frame.

`BinaryProtocolSerializer` encodes one complete frame:

```text
[version:4][id:4][correlationId:4][payloadSize:4][payload][crc32:4]
```

The 32-bit fields are little-endian. `ProtocolVersion.hpp` defines
`PROTOCOL_VERSION`; `Crc32` computes the frame checksum. Deserialization returns
a default `Message` when the frame is malformed, has a different protocol
version, or fails CRC validation.

`ProtocolValidator` is an explicit utility; serialization and `CommClient` do
not invoke it automatically. `ProtocolCompatibility` currently accepts only
equal versions and does not support fallback. `MessageQueue` orders messages by
priority and retains insertion order among equal priorities.

## Utilities

`ErrorCode`, `ResponseHandler`, `ErrorHandler`, `Logger` and `RetryPolicy`
provide communication results, response mapping, logging and reconnect timing.
`ConfigLoader` and `ConnectionConfig` are currently declared in the global
namespace for source compatibility; a failed or unreadable JSON file produces
an empty configuration (empty address and zero numeric fields). Prefer the
`bool load(path, config, errorMessage)` overload to handle failures directly;
the legacy overload logs a warning and returns the empty configuration.

## CMake integration

Use the build-tree alias when embedding the project:

```cmake
add_subdirectory(QtCommsEngine)
target_link_libraries(YourApp PRIVATE QtCommsEngine::QtCommsEngine)
```

The library target propagates its Qt Core and Network dependencies and public
include directory.
