#pragma once

#include <QObject>
#include <QByteArray>

namespace qtcommsengine
{

    /// Abstract transport for sending and receiving raw bytes.
    ///
    /// Implementations are expected to honor the configured timeout for
    /// blocking operations and to be used from their owning QObject thread.
    class CommChannel : public QObject
    {
        Q_OBJECT

    public:
        /// Creates a channel with an optional QObject parent.
        explicit CommChannel(QObject *parent = nullptr);
        ~CommChannel() override;

        /// Opens the transport; returns false if the connection cannot be established.
        virtual bool open() = 0;

        /// Closes the transport.
        virtual void close() = 0;

        /// Sets the timeout, in milliseconds, used by blocking I/O operations.
        virtual void setTimeout(int milliseconds) = 0;

        /// Writes raw bytes to the transport.
        virtual bool send(const QByteArray &data) = 0;

        /// Reads up to maxSize bytes; an empty result indicates no data was read.
        virtual QByteArray receive(int maxSize) = 0;

        /// Returns whether the transport is currently connected.
        virtual bool isConnected() const = 0;

    signals:
        /// Emitted when the underlying transport disconnects.
        void disconnected();
    };

}
