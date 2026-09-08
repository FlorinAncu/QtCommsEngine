# QtCommsEngine

QtCommsEngine is a lightweight communication framework built on top of Qt6, providing
message-based client–channel interaction, automatic reconnection, protocol validation,
priority-based message queues, and a clean abstraction layer for custom transport
implementations.

The project is structured similarly to the original CommsEngine, but adapted for Qt
types (`QByteArray`, `QMutex`, `QTcpSocket`, etc.) and Qt’s event-driven model.

---

## Features

- **Message abstraction**
  - Strongly typed message IDs
  - Priority-based message queue
  - Builder utilities for common protocol messages

- **Protocol validation**
  - Payload checks
  - Control-message rules
  - Sentinel-based error reporting

- **CommClient**
  - Automatic reconnect logic
  - Retry policy support
  - Protocol handshake (GetProtocolVersion, Heartbeat)
  - Unified `sendAndHandle()` API

- **Channels**
  - `TcpChannel` – real network transport
  - `MockChannel` – deterministic testing transport

---

## Directory Structure

QtCommsEngine/
│
├── include/qtcommsengine/   # Public headers
├── src/                     # Library implementation
├── tests/                   # Catch2-based unit tests
├── examples/                # Minimal usage examples
└── CMakeLists.txt           # Root build configuration


---

## Building

### Requirements
- CMake ≥ 3.16  
- Qt6 (Core, Network, Test)  
- C++20 compiler  
- Catch2 (fetched automatically via FetchContent)

### Configure & build

cmake -S . -B build
cmake --build build
Running Tests
QtCommsEngine uses Catch2 for unit testing (same structure as CommsEngine).


ctest --test-dir build -C Debug
Tests cover:

MessageQueue behavior

ProtocolValidator rules

CommClient send/handle logic

Automatic reconnection and protocol resync

Examples
A minimal example is available under examples/:


./build/examples/QtCommsEngineExample
It demonstrates:

Creating a client

Connecting to a channel

Sending messages

Handling responses

License
This project is provided as-is for internal and educational use.