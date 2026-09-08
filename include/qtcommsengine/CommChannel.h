#pragma once

#include <QObject>
#include <QByteArray>

namespace qtcommsengine
{

    // Abstract transport interface for sending and receiving raw message bytes.
    class CommChannel : public QObject
    {
        Q_OBJECT

    public:
        explicit CommChannel(QObject *parent = nullptr);
        ~CommChannel() override;

        virtual bool open() = 0;
        virtual void close() = 0;

        virtual void setTimeout(int milliseconds) = 0;

        virtual bool send(const QByteArray &data) = 0;
        virtual QByteArray receive(int maxSize) = 0;

        virtual bool isConnected() const = 0;

    signals:
        void disconnected();
    };

}
