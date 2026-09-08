# QtCommsEngine

QtCommsEngine is a fully Qt-based communication engine designed for structured binary messaging. It provides a complete transport, serialization, validation, and asynchronous client pipeline built entirely on Qt. The library uses Qt types exclusively (QByteArray, QString, QList, QMutex, QWaitCondition, QThread, QJsonDocument) and avoids STL and non-Qt primitive types. All code follows the Allman brace style and uses the namespace `qtcommsengine`.

QtCommsEngine is organized into several layers. The transport layer includes CommChannel (an abstract communication interface), TcpChannel (a TCP/IP implementation using QTcpSocket), and MockChannel (an in-memory test channel). The message layer includes Message (representing id, payload, and priority), MessagePriority (Low, Normal, High, Critical), MessageBuilder (constructs well-known protocol messages), and MessageParser (extracts id and payload from raw frames). The serialization layer includes BinaryProtocolSerializer (binary framing with version, id, payload size, payload, and CRC32), Crc32 (checksum validation), and ProtocolVersion (protocol version constant). The protocol layer includes Protocol (message ID definitions), ProtocolValidator (validates messages), and ProtocolCompatibility (checks version compatibility). The client layer includes CommManager (synchronous communication manager), CommClient (asynchronous client with background sender thread), MessageQueue (thread-safe priority queue), RetryPolicy (retry and reconnect strategy), and ResponseHandler (interprets server responses). Utilities include Logger (minimal stdout logger), ErrorCode (unified error codes), ErrorHandler (logs error codes), ConfigLoader (loads JSON configuration), and ConnectionConfig (configuration struct).

The project structure is simple: an `include/qtcommsengine` directory containing all headers, a `src` directory containing all implementation files, and a top-level CMakeLists.txt. QtCommsEngine builds as a static library.

To build the library, you need CMake 3.16 or newer, Qt 6 (or Qt 5 with minor adjustments), and C++20. Build commands are:

cmake -S . -B build
cmake --build build

This produces the static library `libQtCommsEngine.a`.

To integrate QtCommsEngine into your project, add the directory and link the library in your CMakeLists.txt:

add_subdirectory(QtCommsEngine)
target_link_libraries(YourApp
    QtCommsEngine
    Qt6::Core
    Qt6::Network
)

Include the necessary headers in your C++ code:

#include <qtcommsengine/CommClient.h>
#include <qtcommsengine/TcpChannel.h>
#include <qtcommsengine/MessageBuilder.h>

A minimal usage example:

#include <qtcommsengine/TcpChannel.h>
#include <qtcommsengine/CommClient.h>
#include <qtcommsengine/MessageBuilder.h>

int main()
{
    qtcommsengine::TcpChannel channel("127.0.0.1", 9000);
    qtcommsengine::CommClient client(&channel);
    qtcommsengine::Message ping = qtcommsengine::MessageBuilder::makePing("Hello");
    client.enqueue(ping);
    return 0;
}

QtCommsEngine uses a deterministic binary frame format:

[version:4 bytes][id:4 bytes][payloadSize:4 bytes][payload:payloadSize bytes][crc32:4 bytes]

CRC32 is computed over all fields except the CRC itself.

Coding style rules are strict: Qt-only, Allman braces, no STL, no non-Qt primitive types, no std::vector, no std::string, no uint32_t. The library uses QByteArray, QString, QList, QMutex, QWaitCondition, and the namespace `qtcommsengine`.

Choose your preferred license (MIT, Apache-2.0, BSD, Proprietary).
