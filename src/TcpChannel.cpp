#include "qtcommsengine/TcpChannel.h"

namespace qtcommsengine
{

    TcpChannel::TcpChannel(const QString &host, quint16 port, QObject *parent)
        : CommChannel(parent)
        , m_host(host)
        , m_port(port)
        , m_socket(new QTcpSocket(this))
        , m_connected(false)
        , m_timeoutMs(200)
    {
    }

    TcpChannel::~TcpChannel()
    {
        close();
    }

    bool TcpChannel::open()
    {
        m_socket->connectToHost(m_host, m_port);

        if (!m_socket->waitForConnected(m_timeoutMs))
        {
            m_connected = false;
            return false;
        }

        m_connected = true;
        return true;
    }

    void TcpChannel::close()
    {
        if (m_socket->isOpen())
        {
            m_socket->close();
        }

        m_connected = false;
    }

    void TcpChannel::setTimeout(int milliseconds)
    {
        m_timeoutMs = milliseconds;
    }

    bool TcpChannel::send(const QByteArray &data)
    {
        if (!isConnected())
        {
            return false;
        }

        qint64 written = m_socket->write(data);
        if (written < 0)
        {
            return false;
        }

        return m_socket->waitForBytesWritten(m_timeoutMs);
    }

    QByteArray TcpChannel::receive(int maxSize)
    {
        if (!isConnected())
        {
            return QByteArray();
        }

        if (!m_socket->waitForReadyRead(m_timeoutMs))
        {
            return QByteArray();
        }

        return m_socket->read(maxSize);
    }

    bool TcpChannel::isConnected() const
    {
        return m_socket->state() == QAbstractSocket::ConnectedState;
    }

}
