# QtCommsEngine

QtCommsEngine is a fully Qt-based communication engine designed for structured binary messaging.  
It provides a complete transport layer, serialization, protocol validation, priority-based message
queues, and an asynchronous client with automatic reconnection. The library uses Qt types
exclusively (`QByteArray`, `QString`, `QList`, `QMutex`, `QWaitCondition`, `QThread`,
`QJsonDocument`) and avoids STL and non-Qt primitive types. All code follows the Allman brace
style and uses the namespace `qtcommsengine`.

QtCommsEngine is lightweight, deterministic, and designed for clean integration into Qt
applications that require message-based communication over custom or standard transports.


## Features

### Message Abstraction
- Strongly typed message IDs  
- Priority-based message queue (Low, Normal, High, Critical)  
- `MessageBuilder` utilities for common protocol messages  
- `MessageParser` for extracting ID and payload from binary frames  

### Protocol Validation
- Payload validation rules  
- Control-message rules  
- Sentinel-based error reporting (`CRC_ERROR`, `VERSION_MISMATCH`)  
- Protocol version compatibility checks  

### CommClient
- Background sender thread  
- Automatic reconnection logic  
- Retry policy support  
- Protocol handshake (GetProtocolVersion, Heartbeat)  
- Unified `sendAndHandle()` API  

### Channels
- `CommChannel` – abstract communication interface  
- `TcpChannel` – TCP/IP transport using `QTcpSocket`  
- `MockChannel` – deterministic in-memory transport for testing  

### Serialization
Binary framing format:

[version:4 bytes][id:4 bytes][payloadSize:4 bytes][payload][crc32:4 bytes]

- `BinaryProtocolSerializer` for encoding/decoding frames  
- `Crc32` checksum validation  
- `ProtocolVersion` constant  

### Utilities
- `Logger` – minimal stdout logger  
- `ErrorCode` – unified error codes  
- `ErrorHandler` – error interpretation  
- `ConfigLoader` – JSON configuration loader  
- `ConnectionConfig` – connection configuration struct  

---

## Directory Structure

QtCommsEngine/
│
├── include/qtcommsengine/   # Public headers
├── src/                     # Library implementation
├── tests/                   # Catch2-based unit tests
├── examples/                # Minimal usage examples
└── CMakeLists.txt           # Root build configuration


## Requirements

- CMake ≥ 3.16  
- Qt6 (Core, Network, Test)  
- C++20 compiler  
- Catch2 (fetched automatically via FetchContent)  

---

## Building

cmake -S . -B build
cmake --build build
This produces the static library:

libQtCommsEngine.a

Running Tests
QtCommsEngine uses Catch2 for unit testing.

ctest --test-dir build -C Debug
Tests cover:

MessageQueue behavior

ProtocolValidator rules

CommClient send/handle logic

Automatic reconnection and protocol resynchronization

Usage Example

#include <qtcommsengine/TcpChannel.h>
#include <qtcommsengine/CommClient.h>
#include <qtcommsengine/MessageBuilder.h>

int main()
{
    qtcommsengine::TcpChannel channel("127.0.0.1", 9000);
    qtcommsengine::CommClient client(&channel);

    qtcommsengine::Message ping =
        qtcommsengine::MessageBuilder::makePing("Hello");

    client.enqueue(ping);
    return 0;
}
Integration in Your Project
In your CMakeLists.txt:

cmake
add_subdirectory(QtCommsEngine)

target_link_libraries(YourApp
    QtCommsEngine
    Qt6::Core
    Qt6::Network
)

Include the necessary headers:

#include <qtcommsengine/CommClient.h>
#include <qtcommsengine/TcpChannel.h>
#include <qtcommsengine/MessageBuilder.h>


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