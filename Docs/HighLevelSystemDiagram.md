# High-Level System Diagram

`CommManager` is the direct, blocking interface. `CommClient` adds a worker
thread and a priority queue around the same channel abstraction. `TcpServer`
operates independently as a server-side component.

```text
Application
    |
    | CommManager: blocking send/receive
    | CommClient: enqueue / sendAndHandle
    v
Message framing and correlation
    |
    v
CommChannel
    +-- TcpChannel --> QTcpSocket
    +-- MockChannel --> in-memory test transport

TcpServer --> per-client QTcpSocket sessions and messageReceived signals
```

For client operations, `CommClient` moves its channel to its worker thread and
serializes access there. `CommManager` encodes outgoing messages and buffers
incoming bytes until a complete frame is available. `BinaryProtocolSerializer`
validates the protocol version and CRC while decoding.

The priority queue selects the highest-priority queued message first and
preserves insertion order for equal priorities. On connection loss the client
attempts to reconnect according to `RetryPolicy`; it does not automatically
replay a failed request. Applications implement any required heartbeat,
protocol negotiation and request retry policy.

`TcpServer` accepts client connections asynchronously, assigns a server-local
`ClientId` to each connection, and emits valid decoded messages with the
originating ID. Replies can be routed with `sendToClient()` or `broadcast()`.
