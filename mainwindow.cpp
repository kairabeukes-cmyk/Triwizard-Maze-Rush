#include "mainwindow.h"

#include "mainmenuwidget.h"
#include "highscoreswidget.h"
#include "gamewidget.h"
#include "hostnetworkmanager.h"
#include "clientnetworkmanager.h"
#include "discoveryservice.h"

#include <QStackedWidget>
#include <QMessageBox>
#include <QDebug>
#include <QPushButton>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      screenStack(nullptr),
      mainMenuWidget(nullptr),
      highScoresWidget(nullptr),
      gameWidget(nullptr),
      hostNetworkManager(nullptr),
      clientNetworkManager(nullptr),
      discoveryService(nullptr)
{
    setWindowTitle("Triwizard Maze Rush");
    resize(1000, 700);

    screenStack = new QStackedWidget(this);

    mainMenuWidget = new MainMenuWidget(this);
    highScoresWidget = new HighScoresWidget(this);
    gameWidget = new GameWidget(this);

    screenStack->addWidget(mainMenuWidget);
    screenStack->addWidget(gameWidget);
    screenStack->addWidget(highScoresWidget);

    setCentralWidget(screenStack);

    connect(mainMenuWidget, &MainMenuWidget::singlePlayerClicked,
            this, &MainWindow::startSinglePlayer);

    connect(mainMenuWidget, &MainMenuWidget::hostMultiplayerClicked,
            this, &MainWindow::startHostMultiplayer);

    connect(mainMenuWidget, &MainMenuWidget::joinMultiplayerClicked,
            this, &MainWindow::startJoinMultiplayer);

    connect(mainMenuWidget, &MainMenuWidget::highScoresClicked,
            this, &MainWindow::showHighScores);

    connect(mainMenuWidget, &MainMenuWidget::exitClicked,
            this, &MainWindow::close);

    connect(highScoresWidget, &HighScoresWidget::backClicked,
            this, &MainWindow::showMainMenu);

    connect(gameWidget, &GameWidget::returnToMenuRequested,
            this, &MainWindow::showMainMenu);

    showMainMenu();
}

MainWindow::~MainWindow()
{
    if (hostNetworkManager)
    {
        hostNetworkManager->stop();
        delete hostNetworkManager;
        hostNetworkManager = nullptr;
    }

    if (clientNetworkManager)
    {
        clientNetworkManager->stop();
        delete clientNetworkManager;
        clientNetworkManager = nullptr;
    }

    if (discoveryService)
    {
        discoveryService->stopBroadcasting();
        delete discoveryService;
        discoveryService = nullptr;
    }
}

void MainWindow::showMainMenu()
{
    screenStack->setCurrentWidget(mainMenuWidget);
}

void MainWindow::startSinglePlayer()
{
    gameWidget->resetGame();
    screenStack->setCurrentWidget(gameWidget);
    gameWidget->setFocus();
}

void MainWindow::showHighScores()
{
    highScoresWidget->refreshScores();
    screenStack->setCurrentWidget(highScoresWidget);
}

void MainWindow::startHostMultiplayer()
{
    if (hostNetworkManager)
    {
        hostNetworkManager->stop();
        delete hostNetworkManager;
        hostNetworkManager = nullptr;
    }

    if (discoveryService)
    {
        discoveryService->stopBroadcasting();
        delete discoveryService;
        discoveryService = nullptr;
    }

    hostNetworkManager = new HostNetworkManager(45454, this);

    connect(hostNetworkManager, &HostNetworkManager::statusChanged,
            this, &MainWindow::showNetworkStatus);

    connect(hostNetworkManager, &HostNetworkManager::messageReceived,
            this, &MainWindow::showNetworkMessage);

    connect(hostNetworkManager, &HostNetworkManager::countdownReceived,
            this, &MainWindow::handleCountdown);

    connect(hostNetworkManager, &HostNetworkManager::multiplayerStartRequested,
            this, &MainWindow::startMultiplayerGame);

    connect(gameWidget, &GameWidget::localPlayerPositionChanged,
            hostNetworkManager, &HostNetworkManager::sendHostPlayerPosition);

    connect(hostNetworkManager, &HostNetworkManager::clientPlayerPositionReceived,
            gameWidget, &GameWidget::setRemotePlayerPosition);

    connect(gameWidget, &GameWidget::localGameEventGenerated,
            hostNetworkManager, &HostNetworkManager::sendHostGameEvent);

    connect(hostNetworkManager, &HostNetworkManager::clientGameEventReceived,
            gameWidget, &GameWidget::applyRemoteGameEvent);

    connect(gameWidget, &GameWidget::localPlayerFinished,
            hostNetworkManager, &HostNetworkManager::sendHostFinishTime);

    connect(hostNetworkManager, &HostNetworkManager::clientFinishTimeReceived,
            gameWidget, &GameWidget::setRemoteFinishTime);

    hostNetworkManager->start();

    /*
     * Start UDP discovery broadcasting so other games can find this host.
     */

    discoveryService = new DiscoveryService(this);
    discoveryService->startBroadcasting("Triwizard Host", 45454);

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Host Multiplayer");
    msgBox.setText("Hosting started on port 45454.\n\n"
                   "Discovery broadcast is active.\n\n"
                   "Open a second copy of the game and click Join Multiplayer.\n\n"
                   "When ready, click Start Multiplayer Race.");
    msgBox.setStandardButtons(QMessageBox::Cancel);

    QPushButton *startButton = msgBox.addButton("Start Multiplayer Race",
                                                QMessageBox::AcceptRole);

    msgBox.exec();

    if (msgBox.clickedButton() == startButton)
    {
        screenStack->setCurrentWidget(gameWidget);
        gameWidget->resetGame();
        gameWidget->setFocus();

        hostNetworkManager->startCountdown();
    }
}

void MainWindow::startJoinMultiplayer()
{
    if (clientNetworkManager)
    {
        clientNetworkManager->stop();
        delete clientNetworkManager;
        clientNetworkManager = nullptr;
    }

    DiscoveryService finder;
    QVector<DiscoveredServer> servers = finder.discoverFor(1500);

    QString hostAddress;
    int hostPort = 45454;
    QString discoveryMessage;

    if (!servers.isEmpty())
    {
        hostAddress = servers[0].address;
        hostPort = servers[0].port;

        discoveryMessage = QString("Discovered game:\n%1\nAddress: %2\nPort: %3")
                               .arg(servers[0].name)
                               .arg(hostAddress)
                               .arg(hostPort);
    }
    else
    {
        bool ok = false;

        hostAddress = QInputDialog::getText(this,
                                            "Join Multiplayer",
                                            "No host was discovered.\n\nEnter the host laptop's ipv4 address:",
                                            QLineEdit::Normal,
                                            "",
                                            &ok);

        if (!ok || hostAddress.trimmed().isEmpty())
            return;

        hostAddress = hostAddress.trimmed();

        discoveryMessage = QString("No host was discovered automatically.\n\n"
                                   "Trying manual address:\n%1:%2")
                               .arg(hostAddress)
                               .arg(hostPort);
    }

    clientNetworkManager = new ClientNetworkManager(hostAddress, hostPort, this);

    connect(clientNetworkManager, &ClientNetworkManager::statusChanged,
            this, &MainWindow::showNetworkStatus);

    connect(clientNetworkManager, &ClientNetworkManager::messageReceived,
            this, &MainWindow::showNetworkMessage);

    connect(clientNetworkManager, &ClientNetworkManager::countdownReceived,
            this, &MainWindow::handleCountdown);

    connect(clientNetworkManager, &ClientNetworkManager::multiplayerStartRequested,
            this, &MainWindow::startMultiplayerGame);

    connect(gameWidget, &GameWidget::localPlayerPositionChanged,
            clientNetworkManager, &ClientNetworkManager::sendClientPlayerPosition);

    connect(clientNetworkManager, &ClientNetworkManager::hostPlayerPositionReceived,
            gameWidget, &GameWidget::setRemotePlayerPosition);

    connect(gameWidget, &GameWidget::localGameEventGenerated,
            clientNetworkManager, &ClientNetworkManager::sendClientGameEvent);

    connect(clientNetworkManager, &ClientNetworkManager::hostGameEventReceived,
            gameWidget, &GameWidget::applyRemoteGameEvent);

    connect(gameWidget, &GameWidget::localPlayerFinished,
            clientNetworkManager, &ClientNetworkManager::sendClientFinishTime);

    connect(clientNetworkManager, &ClientNetworkManager::remoteFinishTimeReceived,
            gameWidget, &GameWidget::setRemoteFinishTime);

    clientNetworkManager->start();

    QMessageBox::information(this,
                             "Join Multiplayer",
                             discoveryMessage + "\n\n"
                             "Trying to connect now.\n\n"
                             "Wait for the host to start the race.");
}

void MainWindow::showNetworkStatus(const QString &message)
{
    qDebug() << "[Network status]" << message;
}

void MainWindow::showNetworkMessage(const QString &message)
{
    qDebug() << "[Network message]" << message;
}

void MainWindow::handleCountdown(int number)
{
    screenStack->setCurrentWidget(gameWidget);
    gameWidget->setCountdownDisplay(number);
    gameWidget->setFocus();
}

void MainWindow::startMultiplayerGame()
{
    screenStack->setCurrentWidget(gameWidget);

    if (hostNetworkManager && hostNetworkManager->isConnected())
    {
        gameWidget->startMultiplayerRace(MultiplayerRole::Host);
    }
    else if (clientNetworkManager && clientNetworkManager->isConnected())
    {
        gameWidget->startMultiplayerRace(MultiplayerRole::Client);
    }
    else
    {
        gameWidget->startMultiplayerRace(MultiplayerRole::None);
    }

    gameWidget->setFocus();
}
