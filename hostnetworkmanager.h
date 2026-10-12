#ifndef HOSTNETWORKMANAGER_H
#define HOSTNETWORKMANAGER_H

#include "networkmanager.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QTimer>
#include <QPointF>

// manages the host side of the tcp multiplayer connection
class HostNetworkManager : public NetworkManager
{
    Q_OBJECT

public:
    explicit HostNetworkManager(int port = 45454, QObject *parent = nullptr);
    ~HostNetworkManager();

    void start() override;
    void stop() override;

    void startCountdown();

    void sendHostPlayerPosition(const QPointF &position);
    void sendHostGameEvent(const QString &eventName);
    void sendHostFinishTime(float finishTime);

    int getPort() const;
    int getClientCount() const;

signals:
    void countdownReceived(int number);
    void multiplayerStartRequested();

    void clientPlayerPositionReceived(const QPointF &position);
    void clientGameEventReceived(const QString &eventName);
    void clientFinishTimeReceived(float finishTime);

private slots:
    void handleNewConnection();
    void readClientData();
    void handleClientDisconnected();
    void sendNextCountdownValue();

private:
    QTcpServer *server;
    QVector<QTcpSocket*> clients;
    int port;

    int countdownValue;
    QTimer *countdownTimer;

    void sendMessageToClient(QTcpSocket *client, const QString &message);
    void broadcastMessage(const QString &message);
};

#endif // HOSTNETWORKMANAGER_H
