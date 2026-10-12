#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QString>
#include <QVector>

// stores information about one discovered host
struct DiscoveredServer
{
    QString name;
    QString address;
    int port;

    DiscoveredServer()
        : name("Unknown"),
          address("127.0.0.1"),
          port(45454)
    {
    }

    DiscoveredServer(const QString &name, const QString &address, int port)
        : name(name),
          address(address),
          port(port)
    {
    }
};

// broadcasts and discovers available multiplayer hosts
class DiscoveryService : public QObject
{
    Q_OBJECT

public:
    explicit DiscoveryService(QObject *parent = nullptr);
    ~DiscoveryService();

    void startBroadcasting(const QString &hostName, int gamePort);
    void stopBroadcasting();

    void listenForServers();
    QVector<DiscoveredServer> discoverFor(int milliseconds);

    QVector<DiscoveredServer> getDiscoveredServers() const;
    void clearDiscoveredServers();

signals:
    void serverDiscovered(const DiscoveredServer &server);

private slots:
    void broadcastHostInfo();
    void readPendingDatagrams();

private:
    QUdpSocket *udpSocket;
    QTimer *broadcastTimer;

    QVector<DiscoveredServer> discoveredServers;

    QString hostName;
    int gamePort;
    int discoveryPort;

    bool alreadyDiscovered(const QString &address, int port) const;
};

#endif // DISCOVERYSERVICE_H
