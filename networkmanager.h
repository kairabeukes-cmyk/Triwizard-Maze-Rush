#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QString>

// base class for host and client networking
class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    virtual ~NetworkManager();

    virtual void start() = 0;
    virtual void stop() = 0;

    bool isConnected() const;
    QString getStatusMessage() const;

signals:
    void statusChanged(const QString &message);
    void messageReceived(const QString &message);

protected:
    bool connected;
    QString statusMessage;

    void setStatus(const QString &message);
};

#endif // NETWORKMANAGER_H
