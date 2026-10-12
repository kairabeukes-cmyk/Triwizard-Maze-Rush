#include "discoveryservice.h"

#include <QHostAddress>
#include <QNetworkDatagram>
#include <QEventLoop>

DiscoveryService::DiscoveryService(QObject *parent)
    : QObject(parent),
      udpSocket(nullptr),
      broadcastTimer(nullptr),
      hostName("Triwizard Host"),
      gamePort(45454),
      discoveryPort(45455)
{
    udpSocket = new QUdpSocket(this);

    connect(udpSocket, &QUdpSocket::readyRead,
            this, &DiscoveryService::readPendingDatagrams);

    broadcastTimer = new QTimer(this);

    connect(broadcastTimer, &QTimer::timeout,
            this, &DiscoveryService::broadcastHostInfo);
}

DiscoveryService::~DiscoveryService()
{
    stopBroadcasting();
}

void DiscoveryService::startBroadcasting(const QString &hostName, int gamePort)
{
    this->hostName = hostName;
    this->gamePort = gamePort;

    broadcastHostInfo();
    broadcastTimer->start(1000);
}

void DiscoveryService::stopBroadcasting()
{
    if (broadcastTimer)
        broadcastTimer->stop();
}

void DiscoveryService::listenForServers()
{
    if (udpSocket->state() == QAbstractSocket::BoundState)
        return;

    udpSocket->bind(QHostAddress::AnyIPv4,
                    discoveryPort,
                    QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
}

QVector<DiscoveredServer> DiscoveryService::discoverFor(int milliseconds)
{
    clearDiscoveredServers();
    listenForServers();

    QEventLoop loop;

    QTimer::singleShot(milliseconds, &loop, SLOT(quit()));

    loop.exec();

    return discoveredServers;
}

QVector<DiscoveredServer> DiscoveryService::getDiscoveredServers() const
{
    return discoveredServers;
}

void DiscoveryService::clearDiscoveredServers()
{
    discoveredServers.clear();
}

void DiscoveryService::broadcastHostInfo()
{
    QString message = QString("TRIWIZARD_DISCOVERY:%1:%2")
                          .arg(hostName)
                          .arg(gamePort);

    QByteArray data = message.toUtf8();

    udpSocket->writeDatagram(data,
                             QHostAddress::Broadcast,
                             discoveryPort);
}

void DiscoveryService::readPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QHostAddress senderAddress;
        quint16 senderPort = 0;

        QByteArray datagram;
        datagram.resize(static_cast<int>(udpSocket->pendingDatagramSize()));

        udpSocket->readDatagram(datagram.data(),
                                datagram.size(),
                                &senderAddress,
                                &senderPort);

        Q_UNUSED(senderPort);

        QString message = QString::fromUtf8(datagram).trimmed();

        if (!message.startsWith("TRIWIZARD_DISCOVERY:"))
            continue;

        QStringList parts = message.split(":");

        if (parts.size() != 3)
            continue;

        QString discoveredName = parts[1];

        bool ok = false;
        int discoveredPort = parts[2].toInt(&ok);

        if (!ok)
            continue;

        QString address = senderAddress.toString();

        // simplify localhost discovery
        if (address == "::1")
            address = "127.0.0.1";

        if (alreadyDiscovered(address, discoveredPort))
            continue;

        DiscoveredServer server(discoveredName, address, discoveredPort);

        discoveredServers.append(server);

        emit serverDiscovered(server);
    }
}

bool DiscoveryService::alreadyDiscovered(const QString &address, int port) const
{
    for (int i = 0; i < discoveredServers.size(); ++i)
    {
        if (discoveredServers[i].address == address &&
            discoveredServers[i].port == port)
        {
            return true;
        }
    }

    return false;
}
