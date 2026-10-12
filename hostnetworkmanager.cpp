#include "hostnetworkmanager.h"

#include <QHostAddress>
#include <QTimer>

HostNetworkManager::HostNetworkManager(int port, QObject *parent)
    : NetworkManager(parent),
      server(nullptr),
      port(port),
      countdownValue(3),
      countdownTimer(nullptr)
{
    server = new QTcpServer(this);

    connect(server, &QTcpServer::newConnection,
            this, &HostNetworkManager::handleNewConnection);

    countdownTimer = new QTimer(this);

    connect(countdownTimer, &QTimer::timeout,
            this, &HostNetworkManager::sendNextCountdownValue);
}

HostNetworkManager::~HostNetworkManager()
{
    stop();
}

void HostNetworkManager::start()
{
    if (server->isListening())
    {
        setStatus(QString("Host already running on port %1").arg(port));
        return;
    }

    bool ok = server->listen(QHostAddress::Any, port);

    if (!ok)
    {
        connected = false;
        setStatus(QString("Failed to host: %1").arg(server->errorString()));
        return;
    }

    connected = true;
    setStatus(QString("Hosting game on port %1").arg(port));
}

void HostNetworkManager::stop()
{
    if (countdownTimer)
        countdownTimer->stop();

    for (int i = 0; i < clients.size(); ++i)
    {
        if (clients[i])
        {
            clients[i]->disconnectFromHost();
            clients[i]->deleteLater();
        }
    }

    clients.clear();

    if (server && server->isListening())
        server->close();

    connected = false;
    setStatus("Host stopped");
}

void HostNetworkManager::startCountdown()
{
    if (!server->isListening())
    {
        setStatus("Cannot start countdown: host is not running");
        return;
    }

    countdownValue = 3;

    broadcastMessage("COUNTDOWN:3\n");
    emit countdownReceived(3);

    setStatus("Countdown started");

    countdownTimer->start(1000);
}

void HostNetworkManager::sendHostPlayerPosition(const QPointF &position)
{
    if (!server->isListening())
        return;

    QString message = QString("HOST_PLAYER:%1:%2\n")
                          .arg(position.x(), 0, 'f', 2)
                          .arg(position.y(), 0, 'f', 2);

    broadcastMessage(message);
}

void HostNetworkManager::sendHostGameEvent(const QString &eventName)
{
    if (!server->isListening())
        return;

    broadcastMessage(QString("HOST_EVENT:%1\n").arg(eventName));
}

void HostNetworkManager::sendHostFinishTime(float finishTime)
{
    if (!server->isListening())
        return;

    broadcastMessage(QString("HOST_FINISHED:%1\n").arg(finishTime, 0, 'f', 2));
}

int HostNetworkManager::getPort() const
{
    return port;
}

int HostNetworkManager::getClientCount() const
{
    return clients.size();
}

void HostNetworkManager::handleNewConnection()
{
    while (server->hasPendingConnections())
    {
        QTcpSocket *client = server->nextPendingConnection();

        clients.append(client);

        connect(client, &QTcpSocket::readyRead,
                this, &HostNetworkManager::readClientData);

        connect(client, &QTcpSocket::disconnected,
                this, &HostNetworkManager::handleClientDisconnected);

        QString message = QString("Client connected. Total clients: %1")
                              .arg(clients.size());

        setStatus(message);
        emit messageReceived(message);

        sendMessageToClient(client, "WELCOME_FROM_HOST\n");
    }
}

void HostNetworkManager::readClientData()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());

    if (!client)
        return;

    while (client->canReadLine())
    {
        QString line = QString::fromUtf8(client->readLine()).trimmed();

        emit messageReceived(QString("Host received: %1").arg(line));

        if (line == "HELLO_FROM_CLIENT")
        {
            sendMessageToClient(client, "JOIN_ACCEPTED\n");
        }
        else if (line.startsWith("CLIENT_PLAYER:"))
        {
            QStringList parts = line.split(":");

            if (parts.size() == 3)
            {
                bool okX = false;
                bool okY = false;

                double x = parts[1].toDouble(&okX);
                double y = parts[2].toDouble(&okY);

                if (okX && okY)
                {
                    QPointF clientPosition(x, y);

                    emit clientPlayerPositionReceived(clientPosition);
                    broadcastMessage(line + "\n");
                }
            }
        }
        else if (line.startsWith("CLIENT_EVENT:"))
        {
            QString eventName = line.section(":", 1, 1);

            emit clientGameEventReceived(eventName);

            // send the client event back out as a host event
            broadcastMessage(QString("HOST_EVENT:%1\n").arg(eventName));
        }
        else if (line.startsWith("CLIENT_FINISHED:"))
        {
            QString timeText = line.section(":", 1, 1);

            bool ok = false;
            float finishTime = timeText.toFloat(&ok);

            if (ok)
            {
                emit clientFinishTimeReceived(finishTime);
                broadcastMessage(QString("CLIENT_FINISHED:%1\n").arg(finishTime, 0, 'f', 2));
            }
        }
    }
}

void HostNetworkManager::handleClientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());

    if (!client)
        return;

    clients.removeAll(client);
    client->deleteLater();

    QString message = QString("Client disconnected. Total clients: %1")
                          .arg(clients.size());

    setStatus(message);
    emit messageReceived(message);
}

void HostNetworkManager::sendNextCountdownValue()
{
    countdownValue--;

    if (countdownValue > 0)
    {
        broadcastMessage(QString("COUNTDOWN:%1\n").arg(countdownValue));
        emit countdownReceived(countdownValue);
        return;
    }

    if (countdownValue == 0)
    {
        broadcastMessage("COUNTDOWN:0\n");
        emit countdownReceived(0);
        return;
    }

    countdownTimer->stop();

    broadcastMessage("START_GAME\n");

    emit multiplayerStartRequested();

    setStatus("Multiplayer race started");
}

void HostNetworkManager::sendMessageToClient(QTcpSocket *client, const QString &message)
{
    if (!client)
        return;

    client->write(message.toUtf8());
    client->flush();
}

void HostNetworkManager::broadcastMessage(const QString &message)
{
    for (int i = 0; i < clients.size(); ++i)
        sendMessageToClient(clients[i], message);
}
