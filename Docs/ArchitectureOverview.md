# Architecture Overview

QtCommsEngine is a Qt 6/C++20 library for exchanging framed binary messages
over TCP or a caller-provided transport. Its components are separated into
transport, runtime, protocol and utility layers.

## Transport

`CommChannel` abstracts raw byte I/O. `TcpChannel` implements a blocking
`QTcpSocket` client, while `MockChannel` provides an in-memory implementation
for tests and examples. `TcpServer` is a separate asynchronous, multi-client
server with per-client IDs and message routing.

## Runtime

`CommManager` serializes and deserializes messages over a non-owned channel and
retains incomplete or coalesced incoming frames between reads. It is intended
for use from the channel's owning thread.

`CommClient` moves its channel to a dedicated worker thread. Its synchronous
connection and request methods submit work to that thread; `enqueue()` provides
priority-ordered asynchronous request processing. Connection loss triggers
reconnection attempts configured by `RetryPolicy`. The client does not replay a
failed request, perform a protocol handshake, or generate periodic heartbeats.

## Protocol

`Message` carries an integer ID, a byte payload, a correlation ID and a
priority. `BinaryProtocolSerializer` encodes and decodes complete frames with
little-endian 32-bit fields and a CRC32 checksum. `MessageBuilder`,
`MessageParser`, `ProtocolValidator` and `ProtocolCompatibility` provide
convenience and protocol utilities.

Validation is opt-in: the client and serializer do not call
`ProtocolValidator::validate()` automatically. The compatibility helper
currently accepts equal protocol versions only; fallback is not implemented.

## Utilities

`ErrorCode`, `ResponseHandler`, `ErrorHandler`, `Logger` and `RetryPolicy`
provide result classification, basic response mapping, logging and reconnect
timing. `ConfigLoader` reads a JSON file into `ConnectionConfig`.

For exact public signatures and usage constraints, see [APIReference.md](./APIReference.md).
