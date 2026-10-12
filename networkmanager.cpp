#include "networkmanager.h"

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent),
      connected(false),
      statusMessage("Not connected")
{
}

NetworkManager::~NetworkManager()
{
}

bool NetworkManager::isConnected() const
{
    return connected;
}

QString NetworkManager::getStatusMessage() const
{
    return statusMessage;
}

void NetworkManager::setStatus(const QString &message)
{
    statusMessage = message;
    emit statusChanged(message);
}
