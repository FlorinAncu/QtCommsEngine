#pragma once

#include <QByteArray>
#include <QtGlobal>

#include "qtcommsengine/MessagePriority.h"

namespace qtcommsengine
{

    // Represents a single protocol message with an id, a payload and a priority.
    class Message
    {
    public:
        Message();
        Message(qint32 id, const QByteArray &payload);

        qint32 getId() const;
        const QByteArray &getPayload() const;

        void setPriority(MessagePriority priority);
        MessagePriority getPriority() const;

    private:
        qint32 m_id;
        QByteArray m_payload;
        MessagePriority m_priority;
    };

}
