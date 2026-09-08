#pragma once

#include <QObject>

#include "qtcommsengine/CommChannel.h"
#include "qtcommsengine/Message.h"

namespace qtcommsengine
{

    // Owns a CommChannel and serializes access to connect/send/receive operations.
    class CommManager : public QObject
    {
        Q_OBJECT

    public:
        explicit CommManager(CommChannel *channel, QObject *parent = nullptr);

        bool connect();
        void disconnect();

        void setTimeout(int milliseconds);
        bool isConnected() const;

        bool sendMessage(const Message &msg);
        bool receiveMessage(Message &msg);

    protected:
        CommChannel *transport() const;

    private:
        CommChannel *m_channel;
        int m_timeoutMs;
    };

}
