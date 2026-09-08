=========================================
CommsEngineDoc.md
=========================================

QtCommsEngine is a standalone, reusable communication runtime designed to provide a robust and fault‑tolerant binary messaging layer for Qt‑based distributed systems. It is fully domain‑agnostic and can be integrated into any application that requires reliable message exchange over a transport channel. The library is built entirely on Qt primitives such as QTcpSocket, QThread, QMutex, QWaitCondition, QByteArray and QString, and exposes a clean public API under the namespace qtcommsengine.

QtCommsEngine implements transport abstraction, binary protocol serialization and deserialization, versioning and compatibility checks, priority‑based message queuing, automatic reconnection, protocol resynchronization, retry logic, timeout management, heartbeat generation and a dedicated sender thread. The architecture is strictly layered and separates transport, runtime, protocol and infrastructure responsibilities.

The project structure consists of an include directory containing all public headers, a src directory containing implementation files, a tests directory containing Qt Test‑based unit tests and an examples directory containing minimal usage examples. The library is built as a static library and integrates cleanly into any CMake‑based Qt project.

The high‑level architecture begins with CommClient, which owns the sender thread, the message queue, the retry logic, the reconnection logic and the protocol resynchronization logic. CommClient delegates connection lifecycle operations to CommManager, which ensures that all transport operations are serialized and protected by mutexes. CommManager delegates raw binary I/O to CommChannel, which is implemented by TcpChannel for real TCP communication and by MockChannel for deterministic in‑memory testing.

The message lifecycle begins when the application enqueues a Message into CommClient. The message is stored in MessageQueue with priority. The sender thread pops the next message, validates it, serializes it using BinaryProtocolSerializer and sends it through CommManager. The transport receives the response, which is parsed by MessageParser, validated by ProtocolValidator and interpreted by ResponseHandler. CommClient returns success or failure to the application.

Automatic reconnection and retry logic ensure that communication remains stable even when the underlying network is unreliable. If a send attempt fails, CommClient disconnects, attempts to reconnect, performs protocol resynchronization and retries the message according to RetryPolicy. Heartbeat messages are generated periodically to maintain connection liveness and detect failures early.

The protocol layer defines the binary message format, the protocol version constant, the compatibility rules, the validation rules and the CRC integrity checks. Messages consist of an integer identifier and a QByteArray payload. BinaryProtocolSerializer produces deterministic binary frames that contain version, id, payload size, payload and CRC32. ProtocolCompatibility determines whether two protocol versions can communicate or whether fallback is required.

The infrastructure layer provides logging, error categorization, error handling, response interpretation and retry configuration. Logger outputs informational, warning and error messages. ErrorCode enumerates common error identifiers. ErrorHandler processes error codes. ResponseHandler interprets server responses and maps them to error conditions. RetryPolicy defines maximum retries, retry delay and reconnect delay.

All public API resides under the namespace qtcommsengine. The library uses Qt types exclusively and avoids STL containers, STL strings and STL threading primitives. This ensures consistency with Qt’s event‑driven architecture and avoids mixing incompatible concurrency models.

QtCommsEngine is designed to be extensible. New transports can be added by implementing CommChannel. New message types can be introduced by extending MessageBuilder. New protocol versions can be defined by updating ProtocolVersion and ProtocolCompatibility. Custom logging and error handling can be integrated by replacing Logger and ErrorHandler. The architecture is suitable for industrial communication systems that require deterministic behavior, strict validation and robust error handling.

=========================================
End of  CommsEngineDoc.md
=========================================