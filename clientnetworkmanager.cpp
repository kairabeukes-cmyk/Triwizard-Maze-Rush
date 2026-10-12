#include "clientnetworkmanager.h"

ClientNetworkManager::ClientNetworkManager(const QString &hostAddress,
                                           int port,
                                           QObject *parent)
    : NetworkManager(parent),
      socket(nullptr),
      hostAddress(hostAddress),
      port(port)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected,
            this, &ClientNetworkManager::handleConnected);

    connect(socket, &QTcpSocket::disconnected,
            this, &ClientNetworkManager::handleDisconnected);

    connect(socket, &QTcpSocket::readyRead,
            this, &ClientNetworkManager::readServerData);

    connect(socket,
            QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this,
            &ClientNetworkManager::handleError);
}

ClientNetworkManager::~ClientNetworkManager()
{
    stop();
}

void ClientNetworkManager::start()
{
    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        setStatus("Client already connected");
        return;
    }

    setStatus(QString("Connecting to %1:%2").arg(hostAddress).arg(port));

    socket->connectToHost(hostAddress, port);
}

void ClientNetworkManager::stop()
{
    if (socket->state() != QAbstractSocket::UnconnectedState)
        socket->disconnectFromHost();

    connected = false;
    setStatus("Client stopped");
}

void ClientNetworkManager::sendTestMessage(const QString &message)
{
    if (socket->state() != QAbstractSocket::ConnectedState)
    {
        setStatus("Cannot send message: client is not connected");
        return;
    }

    QString packet = message + "\n";

    socket->write(packet.toUtf8());
    socket->flush();
}

void ClientNetworkManager::sendClientPlayerPosition(const QPointF &position)
{
    if (socket->state() != QAbstractSocket::ConnectedState)
        return;

    QString message = QString("CLIENT_PLAYER:%1:%2\n")
                          .arg(position.x(), 0, 'f', 2)
                          .arg(position.y(), 0, 'f', 2);

    socket->write(message.toUtf8());
    socket->flush();
}

void ClientNetworkManager::sendClientGameEvent(const QString &eventName)
{
    if (socket->state() != QAbstractSocket::ConnectedState)
        return;

    QString message = QString("CLIENT_EVENT:%1\n").arg(eventName);

    socket->write(message.toUtf8());
    socket->flush();
}

void ClientNetworkManager::sendClientFinishTime(float finishTime)
{
    if (socket->state() != QAbstractSocket::ConnectedState)
        return;

    QString message = QString("CLIENT_FINISHED:%1\n").arg(finishTime, 0, 'f', 2);

    socket->write(message.toUtf8());
    socket->flush();
}

QString ClientNetworkManager::getHostAddress() const
{
    return hostAddress;
}

int ClientNetworkManager::getPort() const
{
    return port;
}

void ClientNetworkManager::handleConnected()
{
    connected = true;
    setStatus(QString("Connected to host %1:%2").arg(hostAddress).arg(port));

    sendTestMessage("HELLO_FROM_CLIENT");
}

void ClientNetworkManager::handleDisconnected()
{
    connected = false;
    setStatus("Disconnected from host");
}

void ClientNetworkManager::readServerData()
{
    while (socket->canReadLine())
    {
        QString line = QString::fromUtf8(socket->readLine()).trimmed();

        emit messageReceived(QString("Client received: %1").arg(line));

        if (line.startsWith("COUNTDOWN:"))
        {
            QString numberText = line.section(":", 1, 1);

            bool ok = false;
            int number = numberText.toInt(&ok);

            if (ok)
                emit countdownReceived(number);
        }
        else if (line == "START_GAME")
        {
            emit multiplayerStartRequested();
        }
        else if (line.startsWith("HOST_PLAYER:"))
        {
            QStringList parts = line.split(":");

            if (parts.size() == 3)
            {
                bool okX = false;
                bool okY = false;

                double x = parts[1].toDouble(&okX);
                double y = parts[2].toDouble(&okY);

                if (okX && okY)
                    emit hostPlayerPositionReceived(QPointF(x, y));
            }
        }
        else if (line.startsWith("HOST_EVENT:"))
        {
            QString eventName = line.section(":", 1, 1);
            emit hostGameEventReceived(eventName);
        }
        else if (line.startsWith("HOST_FINISHED:") ||
                 line.startsWith("CLIENT_FINISHED:"))
        {
            QString timeText = line.section(":", 1, 1);

            bool ok = false;
            float finishTime = timeText.toFloat(&ok);

            if (ok)
                emit remoteFinishTimeReceived(finishTime);
        }
    }
}

void ClientNetworkManager::handleError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);

    connected = false;

    setStatus(QString("Client network error: %1").arg(socket->errorString()));
}
