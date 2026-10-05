# QtCommsEngine

QtCommsEngine is a Qt-based communication engine for structured binary messaging.
It provides TCP and mock transports, serialization, optional protocol validation,
priority-based message queues, and a client with automatic reconnection. It uses
Qt types for messages and transport, with a small amount of standard C++ for
thread synchronization. The library uses the namespace `qtcommsengine`.

QtCommsEngine is lightweight, deterministic, and designed for clean integration into Qt
applications that require message-based communication over custom or standard transports.


## Features

### Message Abstraction
- Strongly typed message IDs  
- Priority-based message queue (Low, Normal, High, Critical)  
- `MessageBuilder` utilities for common protocol messages  
- `MessageParser` for decoding versioned, CRC-protected binary frames  

### Protocol Validation
- `ProtocolValidator::validate()` checks message IDs and payload rules when called
- Sentinel payloads (`CRC_ERROR`, `VERSION_MISMATCH`) are recognized by the validator
- The binary decoder independently rejects incorrect versions and CRCs

### CommClient
- Worker thread for socket operations, including synchronous `connect()`,
    `disconnect()`, and `sendAndHandle()` calls
- `enqueue()` for asynchronous requests and `messageReceived` for incoming messages
- `enqueueResponse()` for replies to server-initiated requests
- Automatic reconnection using `RetryPolicy`; GetProtocolVersion and Heartbeat
    are available as requests, not an automatic handshake

### Channels
- `CommChannel` – abstract communication interface  
- `TcpChannel` – TCP/IP transport using `QTcpSocket`  
- `TcpServer` – multi-client TCP server with framed message reception and per-client routing
- `MockChannel` – deterministic in-memory transport for testing  

`TcpChannel` accepts IPv4 and IPv6 addresses (as well as hostnames). `TcpServer`
accepts numeric IPv4 or IPv6 bind addresses. Its wildcard values (`""`, `"*"`,
and `"0.0.0.0"`) listen on IPv6-any and IPv4-any on the same port. If the
operating system's IPv6 listener already accepts IPv4 clients, the separate
IPv4 listener may not be needed; otherwise it provides IPv4 access directly.

`TcpServer` assigns each accepted connection a `ClientId`. The `clientConnected()` and
`clientDisconnected()` signals identify the affected connection, and `messageReceived()`
provides both the sender ID and message. Use `sendToClient()` or `broadcast()` to route
outgoing messages. `clientIds()` returns connected IDs in unspecified order.
`setMaximumClients()` limits active connections; `0` means unlimited, and additional
connections are closed when a positive limit has been reached. Lowering the limit does
not disconnect existing clients. Negative limits are rejected. `hasClients()` checks
whether at least one connection is active. `send()` succeeds only when exactly one
client is connected; `sendToClient()` fails for an unknown or disconnected ID, and
`broadcast()` fails if there are no clients or any write fails. Use `sendToClient()` or
`broadcast()` when multiple clients are allowed.

This API update is source-incompatible with the former single-client API:
`hasClient()` is now `hasClients()`, and the connection and message signals now include
a `ClientId`. Update existing signal handlers to accept the ID (or use a lambda that
ignores it). Applications configured with a maximum of one client can continue to use
`send()` without changing its routing behavior.

```cpp
connect(&server, &TcpServer::messageReceived, this,
        [&server](TcpServer::ClientId clientId, const Message &request)
        {
            Message reply(/* response ID */, request.getPayload());
            reply.setCorrelationId(request.getCorrelationId());
            server.sendToClient(clientId, reply);
        });
```

Client IDs identify connections only for the lifetime of that server instance; obtain
the ID from the connection or message signals rather than relying on their numeric value.

### Serialization
Protocol version 2 binary framing format (32-bit fields are little-endian):

```text
[version:4][id:4][correlationId:4][payloadSize:4][payload][crc32:4]
```

Version 2 is not wire-compatible with version 1: upgrade both peers together.
Each request receives a nonzero correlation ID; replies must copy that ID.
`CommClient` generates IDs in the lower half of the range, while the example
IndustrialDataEcosystem server generates IDs in the upper half. A response
completes a client request only when both its message type and correlation ID
match (an `Error` with the same correlation ID also completes it). No pending
request is matched to a response with a different ID; queued requests can be
sent after reconnection.

- `BinaryProtocolSerializer` for encoding/decoding frames  
- `Crc32` checksum validation  
- `PROTOCOL_VERSION` constant  

### Utilities
- `Logger` – minimal stdout logger  
- `ErrorCode` – unified error codes  
- `ErrorHandler` – error interpretation  
- `ConfigLoader` – JSON configuration loader  
- `ConnectionConfig` – connection configuration struct  

---

## Directory Structure

```text
QtCommsEngine/
├── include/qtcommsengine/   # Public headers
├── src/                     # Library implementation
├── tests/                   # Catch2-based unit tests
├── examples/                # Minimal usage examples
└── CMakeLists.txt           # Root build configuration
```


## Requirements

- CMake ≥ 3.16  
- Qt6 (Core, Network, Test)  
- C++20 compiler  
- Catch2 (fetched via FetchContent when tests are enabled)  

---

## Building

```sh
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

The build produces a static library (`QtCommsEngine.lib` on Windows, or
`libQtCommsEngine.a` on platforms using the GNU archive format). Catch2 tests
cover queues, validation, framing, correlation IDs, synchronous and asynchronous
message exchange, and reconnection.

## Usage Example

A version 2 server must already be listening on port 9000. This synchronous
example waits for the reply before the client goes out of scope:

```cpp
#include <qtcommsengine/CommClient.hpp>
#include <qtcommsengine/MessageBuilder.hpp>
#include <qtcommsengine/TcpChannel.hpp>

int main()
{
    qtcommsengine::TcpChannel channel("127.0.0.1", 9000);
    qtcommsengine::CommClient client(&channel);

    if (!client.connect())
    {
        return 1;
    }

    return client.sendAndHandle(qtcommsengine::MessageBuilder::makePing("Hello"))
        == qtcommsengine::ErrorCode::Ok ? 0 : 1;
}
```

For asynchronous use, connect to `CommClient::messageReceived`, then call
`enqueue()`. Keep the client alive while the request is pending; destroying or
stopping it does not drain queued messages.

## Integration in Your Project

```cmake
set(QTCOMMSENGINE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(QTCOMMSENGINE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(QtCommsEngine)
target_link_libraries(YourApp PRIVATE QtCommsEngine Qt6::Core Qt6::Network)
```

Include public headers from `<qtcommsengine/...>`.


License

MIT License

Copyright (c) 2026 Florin Ancu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

This project is provided as-is for internal and educational use.