#pragma once

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "qtcommsengine/Message.hpp"

class QTcpServer;
class QTcpSocket;

namespace qtcommsengine
{

    class TcpServer : public QObject
    {
        Q_OBJECT

    public:
        TcpServer(const QString &host, quint16 port, QObject *parent = nullptr);

        bool listen();
        void close();
        bool isListening() const;
        bool hasClient() const;
        bool send(const Message &message);
        void setHost(const QString &host);
        void setPort(quint16 port);

    signals:
        void messageReceived(const qtcommsengine::Message &message);
        void clientConnected();
        void clientDisconnected();
        void protocolError(const QString &message);

    private slots:
        void acceptPendingConnection();
        void readClientData();
        void handleClientDisconnected();

    private:
        QString m_host;
        quint16 m_port;
        QTcpServer *m_server;
        QTcpServer *m_ipv4Server;
        QTcpSocket *m_client;
        QByteArray m_receiveBuffer;
    };

}