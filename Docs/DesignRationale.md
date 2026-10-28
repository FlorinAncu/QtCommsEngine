# Design Rationale

## Transport boundary

`CommChannel` keeps protocol code independent of the byte transport. The
included `TcpChannel` supports network use, while `MockChannel` provides a
deterministic substitute for examples and tests. The standalone `TcpServer`
supports applications that need to accept multiple client connections.

## Framed messages

Messages use a length-delimited binary frame with a protocol version, message
ID, correlation ID, payload length and CRC32. Explicit field widths and
little-endian encoding make the wire representation deterministic. The frame
decoder rejects malformed frames, unsupported versions and checksum failures.

## Threading

The direct `CommManager` API performs blocking I/O and follows the transport's
QObject thread affinity. `CommClient` provides a dedicated worker thread so
callers can submit work without invoking the channel from the wrong thread.
Incoming messages are reported through a Qt signal; the application remains
responsible for higher-level behavior such as periodic heartbeats and version
negotiation.

## Scope and extension

`RetryPolicy` controls connection retry timing. A failed request is not
automatically replayed because the peer may have processed it before the
connection failed. Message validation is available through
`ProtocolValidator`, but applications decide when and where to invoke it.

New transports can implement `CommChannel`; protocol-specific behavior can be
extended through message IDs and application-level handlers. Only Qt Core and
Network are required to build the library; Catch2 is used by the optional tests.
