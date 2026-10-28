#pragma once

#include <QObject>

#include "qtcommsengine/CommChannel.hpp"
#include "qtcommsengine/Message.hpp"

namespace qtcommsengine
{

    /// Provides message-level operations over a non-owned communication channel.
    ///
    /// The channel must outlive the manager. Calls are blocking and must be made
    /// from the channel's owning thread.
    class CommManager : public QObject
    {
        Q_OBJECT

    public:
        /// Creates a manager for channel. A null channel is permitted but operations fail.
        explicit CommManager(CommChannel *channel, QObject *parent = nullptr);

        /// Opens the channel.
        virtual bool connect();

        /// Closes the channel and clears any buffered incoming bytes.
        virtual void disconnect();

        /// Sets the default timeout for channel operations, in milliseconds.
        void setTimeout(int milliseconds);

        /// Returns whether the channel is connected.
        bool isConnected() const;

        /// Serializes and sends one message; returns false if disconnected or sending fails.
        bool sendMessage(const Message &msg);

        /// Receives and decodes one complete message.
        /// A negative timeout uses the manager's configured default.
        bool receiveMessage(Message &msg, int timeoutMs = -1);

    protected:
        CommChannel *transport() const;

    private:
        CommChannel *m_channel;
        int m_timeoutMs;
        QByteArray m_receiveBuffer;
    };

}
