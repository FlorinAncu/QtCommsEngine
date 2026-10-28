# QtCommsEngine

QtCommsEngine is a reusable Qt 6 library for framed binary messaging. It
provides a TCP client channel, an asynchronous multi-client TCP server, a mock
channel, message serialization, a priority queue and a worker-thread client.

The implementation currently provides connection retry after connection loss,
but does not replay failed requests or run an automatic heartbeat or protocol
version handshake. `ProtocolValidator` and `ProtocolCompatibility` are
available as explicit utilities; they are not automatically applied to every
message.

The library is built with CMake and requires Qt Core and Network. Catch2 is
fetched by CMake when tests are enabled. Start with the [README](../README.md)
for build and integration instructions, and see
[APIReference.md](./APIReference.md) for the public API and its constraints.
