=========================================
APIReference.md
=========================================
QtCommsEngine provides a complete Qt‑based communication runtime built on QTcpSocket, QThread, QMutex, QWaitCondition and QByteArray. The library exposes a fully structured API organized into four layers: Transport Layer, Runtime Layer, Protocol Layer and Infrastructure Layer. All public API resides under the namespace qtcommsengine and uses Qt types exclusively.

Transport Layer

CommChannel is the abstract interface for all transport channels. It defines open(), close(), isConnected(), setTimeout(int), send(const QByteArray&) and receive(int). TcpChannel implements real TCP communication using QTcpSocket. It manages connection establishment, disconnection, blocking send and receive operations, and timeout handling. MockChannel is an in‑memory deterministic transport used for testing. It stores the last sent buffer and returns predefined responses without performing any network I/O.

Runtime Layer

CommManager controls the connection lifecycle. It provides connect(), disconnect(), isConnected(), setTimeout(int), sendMessage(const Message&) and receiveMessage(Message&). CommClient extends CommManager and implements the high‑level communication engine. It provides enqueue(const Message&), sendAndHandle(const Message&), trySendOnce(const Message&), runSender() and reconnectAndResynchronize(). CommClient features automatic reconnection, retry logic, heartbeat, protocol resynchronization and a dedicated sender thread implemented using QThread. It uses MessageQueue to manage prioritized message dispatching.

Message Model

Message represents a binary protocol message. It contains an integer id, a QByteArray payload and a MessagePriority value. MessageBuilder constructs well‑known protocol messages such as Ping, Heartbeat, GetStatus, SetParameter and GetProtocolVersion. MessageParser extracts id and payload from raw binary buffers. MessageQueue is a thread‑safe priority queue implemented using QList, QMutex and QWaitCondition. It provides push(const Message&), pop(Message&) and close().

Protocol Layer

ProtocolVersion defines the protocol version constant. ProtocolCompatibility checks whether two versions are compatible or whether fallback is possible. ProtocolValidator validates messages against protocol rules. BinaryProtocolSerializer serializes messages into binary frames and deserializes them back into Message objects. Crc32 computes CRC32 checksums for integrity validation.

Infrastructure Layer

ErrorCode enumerates common error identifiers. ErrorHandler logs error codes. ResponseHandler interprets server responses and maps them to ErrorCode. Logger provides info(), warn() and error() logging functions. RetryPolicy defines retry configuration fields: maxRetries, retryDelayMs and reconnectDelayMs.

This API reference describes all public components required to build applications using QtCommsEngine.

=========================================
End of APIReference.md
=========================================