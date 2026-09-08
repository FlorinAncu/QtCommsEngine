=========================================
DesignRationale.md
=========================================

QtCommsEngine is designed as a robust and reusable communication runtime that can be embedded into any Qt‑based application requiring structured binary messaging over a transport channel. The architecture follows strict separation of concerns and organizes the system into four layers: Transport Layer, Runtime Layer, Protocol Layer and Infrastructure Layer. This separation ensures clear responsibilities, modular extensibility, predictable behavior and simplified testing.

The transport abstraction is provided by CommChannel, which defines the minimal interface required for any communication transport. This allows QtCommsEngine to support multiple transports such as TCP, mock transports for testing and future extensions like serial or UDP. TcpChannel implements real network communication using QTcpSocket, while MockChannel provides deterministic behavior for unit tests without relying on network conditions. This abstraction isolates platform‑specific code and ensures that higher layers remain independent of transport details.

The binary protocol is chosen instead of text‑based formats because it provides deterministic parsing, low overhead, predictable latency and straightforward versioning. BinaryProtocolSerializer defines a strict frame format containing version, identifier, payload size, payload and CRC32. This ensures that messages are compact, efficient and validated for integrity. ProtocolVersion defines the protocol version constant, while ProtocolCompatibility determines whether two versions can communicate or whether fallback is required. ProtocolValidator ensures that messages conform to protocol rules before being processed.

Automatic reconnection and retry logic are essential because real‑world networks are unreliable. CommClient includes retry logic, reconnection logic and protocol resynchronization to ensure that communication remains stable even when the underlying network experiences failures. If a send attempt fails, CommClient disconnects, attempts to reconnect, performs protocol resynchronization and retries the message according to RetryPolicy. This design ensures resilience and minimizes the need for user intervention.

A dedicated sender thread is used to prevent blocking the main application thread. The sender thread handles message dispatching, retries, reconnection and protocol resynchronization. This ensures timely processing of messages and isolates transport operations from the rest of the application. MessageQueue provides prioritized message dispatching, ensuring that protocol‑critical messages such as heartbeat or version negotiation bypass normal traffic and are processed first. This prevents starvation and ensures predictable behavior under load.

Several components such as RetryPolicy and ProtocolVersion are header‑only to simplify integration, reduce compilation complexity and improve performance through inlining. Namespace isolation is achieved by placing all public API under qtcommsengine, preventing symbol collisions and clarifying ownership. QtCommsEngine uses Qt types exclusively and avoids STL containers, STL strings and STL threading primitives to maintain consistency with Qt’s event‑driven architecture and avoid mixing incompatible concurrency models.

Minimal dependencies are used to ensure portability and ease of integration. Only QtCore and QtNetwork are required at runtime. Qt Test is used for unit testing. This design minimizes external requirements and ensures that QtCommsEngine can be integrated into any Qt/CMake project without additional dependencies.

QtCommsEngine is designed to be extensible. New transports can be added by implementing CommChannel. New message types can be introduced by extending MessageBuilder. New protocol versions can be defined by updating ProtocolVersion and ProtocolCompatibility. Custom logging and error handling can be integrated by replacing Logger and ErrorHandler. The architecture is suitable for industrial communication systems that require deterministic behavior, strict validation and robust error handling.

=========================================
Sfârșit DesignRationale.md
=========================================