#ifndef CLIENTNETWORKMANAGER_H
#define CLIENTNETWORKMANAGER_H

#include "networkmanager.h"

#include <QTcpSocket>
#include <QPointF>

// manages the client side of the tcp multiplayer connection
class ClientNetworkManager : public NetworkManager
{
    Q_OBJECT

public:
    explicit ClientNetworkManager(const QString &hostAddress = "127.0.0.1",
                                  int port = 45454,
                                  QObject *parent = nullptr);
    ~ClientNetworkManager();

    void start() override;
    void stop() override;

    void sendTestMessage(const QString &message);
    void sendClientPlayerPosition(const QPointF &position);
    void sendClientGameEvent(const QString &eventName);
    void sendClientFinishTime(float finishTime);

    QString getHostAddress() const;
    int getPort() const;

signals:
    void countdownReceived(int number);
    void multiplayerStartRequested();

    void hostPlayerPositionReceived(const QPointF &position);
    void hostGameEventReceived(const QString &eventName);
    void remoteFinishTimeReceived(float finishTime);

private slots:
    void handleConnected();
    void handleDisconnected();
    void readServerData();
    void handleError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;
    QString hostAddress;
    int port;
};

#endif // CLIENTNETWORKMANAGER_H
