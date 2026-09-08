=========================================
HighLevelSystemDiagram.md
=========================================

QtCommsEngine organizes its communication runtime into a clear and deterministic flow that connects the application layer with the underlying transport layer through a structured set of components. The system is designed to ensure predictable behavior, strict protocol validation and robust error handling. The following description outlines the high‑level structure and data flow of the entire communication stack.

The application interacts only with CommClient. The application enqueues messages into CommClient, which owns the sender thread, the retry logic, the reconnection logic, the protocol resynchronization logic and the priority‑based message queue. CommClient ensures that messages are dispatched in the correct order and that protocol‑critical messages such as heartbeat or version negotiation are processed with higher priority.

CommClient delegates connection lifecycle operations to CommManager. CommManager is responsible for establishing and closing the connection, checking connection state, managing timeouts and serializing all transport operations through mutex protection. CommManager ensures that no concurrent send or receive operations occur on the underlying transport.

CommManager delegates raw binary I/O to CommChannel. CommChannel is the abstract interface that defines open, close, isConnected, send and receive operations. TcpChannel implements real TCP communication using QTcpSocket and provides blocking send and receive operations with timeout handling. MockChannel implements deterministic in‑memory communication for testing and returns predefined responses without performing any network I/O.

The protocol layer operates across all runtime components. Message represents a binary protocol message containing an identifier and a payload. MessageBuilder constructs well‑known protocol messages. MessageParser extracts identifiers and payloads from raw binary buffers. BinaryProtocolSerializer converts Message objects into deterministic binary frames and reconstructs Message objects from binary frames. ProtocolValidator ensures that messages conform to protocol rules. ProtocolVersion defines the protocol version constant. ProtocolCompatibility determines whether two protocol versions can communicate or whether fallback is required. Crc32 computes CRC32 checksums for integrity validation.

The infrastructure layer provides cross‑cutting utilities. Logger outputs informational, warning and error messages. ErrorCode enumerates common error identifiers. ErrorHandler processes error codes. ResponseHandler interprets server responses and maps them to error conditions. RetryPolicy defines maximum retries, retry delay and reconnect delay.

The data flow begins when the application enqueues a message into CommClient. The message is stored in MessageQueue with priority. The sender thread pops the next message and serializes it using BinaryProtocolSerializer. CommManager ensures that the connection is active and that transport operations are protected by mutexes. The serialized binary buffer is sent through CommChannel. The transport receives the response, which is parsed by MessageParser, validated by ProtocolValidator and interpreted by ResponseHandler. CommClient returns success or failure to the application.

This high‑level structure ensures that QtCommsEngine remains deterministic, extensible and robust under real‑world network conditions. The system is designed to support future transports, new protocol versions, custom logging and custom error handling while maintaining a clear and predictable communication flow.


                    +-----------------------------+
                    |         Application         |
                    |     (Your Qt-based code)    |
                    +--------------+--------------+
                                   |
                                   | enqueue(Message)
                                   v
                    +-----------------------------+
                    |         CommClient          |
                    |-----------------------------|
                    | - Sender QThread            |
                    | - Retry logic               |
                    | - Reconnection              |
                    | - Protocol resync           |
                    | - MessageQueue (priority)   |
                    +--------------+--------------+
                                   |
                                   | send/receive via CommManager
                                   v
                    +-----------------------------+
                    |         CommManager         |
                    |-----------------------------|
                    | - connect() / disconnect()  |
                    | - isConnected()             |
                    | - timeout handling          |
                    | - transport mutex           |
                    +--------------+--------------+
                                   |
                                   | virtual send()/receive()
                                   v
                    +-----------------------------+
                    |         CommChannel         |
                    |     (abstract interface)    |
                    +--------------+--------------+
                                   |
                     +-------------+-------------+
                     |                           |
                     v                           v
        +-----------------------+     +-----------------------+
        |       TcpChannel      |     |      MockChannel      |
        |-----------------------|     |-----------------------|
        | - QTcpSocket          |     | - In-memory transport |
        | - Network I/O         |     | - Deterministic tests |
        +-----------------------+     +-----------------------+


Protocol and Message Processing (cross-cutting):

                    +-----------------------------+
                    |          Protocol           |
                    |-----------------------------|
                    | - Message                   |
                    | - MessageBuilder            |
                    | - MessageParser             |
                    | - ProtocolVersion           |
                    | - ProtocolCompatibility     |
                    | - ProtocolValidator         |
                    | - BinaryProtocolSerializer  |
                    | - Crc32                     |
                    +-----------------------------+


Infrastructure (cross-cutting):

                    +-----------------------------+
                    |        Infrastructure       |
                    |-----------------------------|
                    | - Logger                    |
                    | - ErrorCode                 |
                    | - ErrorHandler              |
                    | - ResponseHandler           |
                    | - RetryPolicy               |
                    +-----------------------------+


Data Flow Summary:

The application enqueues a Message into CommClient.

CommClient pushes the message into MessageQueue with priority.

The sender thread pops the next message.

CommClient serializes the message using BinaryProtocolSerializer.

CommManager ensures connection and mutex protection.

CommChannel sends the binary buffer.

CommChannel receives the response.

CommClient parses and validates the response.

ResponseHandler interprets the result.

The application receives success or failure.

=========================================
End of HighLevelSystemDiagram.md
=========================================