#pragma once

#include <QByteArray>
#include <QMutex>

#include "qtcommsengine/CommChannel.hpp"

namespace qtcommsengine
{

    /// In-memory CommChannel that returns canned protocol responses.
    ///
    /// Intended for tests and examples; it performs no network I/O.
    class MockChannel : public CommChannel
    {
        Q_OBJECT

    public:
        /// Creates a disconnected mock channel.
        explicit MockChannel(QObject *parent = nullptr);

        /// Marks the mock channel connected unless open failure is enabled.
        bool open() override;

        /// Marks the mock channel disconnected.
        void close() override;

        /// Stores the timeout; no waiting is performed by this mock.
        void setTimeout(int milliseconds) override;

        /// Decodes a request and prepares its corresponding canned response.
        bool send(const QByteArray &data) override;

        /// Returns up to maxSize bytes from the pending response.
        QByteArray receive(int maxSize) override;

        /// Closes the mock and emits disconnected().
        void forceDisconnect();

        /// Returns the current mock connection state.
        bool isConnected() const override;

        /// Controls whether subsequent open() calls fail.
        void setOpenShouldFail(bool shouldFail);

    private:
        QByteArray m_responseData;
        bool m_connected;
        bool m_openShouldFail = false;
        int m_timeoutMs;
        mutable QMutex m_mutex;
    };

}
