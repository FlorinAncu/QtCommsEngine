#pragma once

#include <QByteArray>
#include <QMutex>

#include "qtcommsengine/CommChannel.h"

namespace qtcommsengine
{

    // In-memory CommChannel stub that echoes canned responses; used for tests and examples.
    class MockChannel : public CommChannel
    {
        Q_OBJECT

    public:
        explicit MockChannel(QObject *parent = nullptr);

        bool open() override;
        void close() override;

        void setTimeout(int milliseconds) override;

        bool send(const QByteArray &data) override;
        QByteArray receive(int maxSize) override;
		
        void forceDisconnect();
        bool isConnected() const override;

        // When true, open() will fail — used to simulate connection errors in tests.
        void setOpenShouldFail(bool shouldFail) { m_openShouldFail = shouldFail; }

    private:
        QByteArray m_responseData;
        bool m_connected;
        bool m_openShouldFail = false;
        int m_timeoutMs;
        mutable QMutex m_mutex;
    };

}
