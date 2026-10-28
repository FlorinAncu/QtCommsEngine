#pragma once

#include <QByteArray>
#include <QtGlobal>

#include "qtcommsengine/MessagePriority.hpp"

namespace qtcommsengine
{

    /// A protocol message with an application-defined ID, payload and priority.
    class Message
    {
    public:
        /// Creates an empty message with ID zero, no correlation ID and normal priority.
        Message();

        /// Creates a message with the given ID and payload.
        Message(qint32 id, const QByteArray &payload);

        /// Returns the message ID.
        qint32 getId() const;

        /// Returns the message payload as raw bytes.
        const QByteArray &getPayload() const;

        /// Returns the request/response correlation ID; zero means unspecified.
        quint32 getCorrelationId() const;

        /// Sets the request/response correlation ID.
        void setCorrelationId(quint32 correlationId);

        /// Sets the priority used by MessageQueue.
        void setPriority(MessagePriority priority);

        /// Returns the message priority.
        MessagePriority getPriority() const;

    private:
        qint32 m_id;
        QByteArray m_payload;
        quint32 m_correlationId = 0;
        MessagePriority m_priority;
    };

}
