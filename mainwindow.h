#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class MainMenuWidget;
class HighScoresWidget;
class GameWidget;
class HostNetworkManager;
class ClientNetworkManager;
class DiscoveryService;

// controls screen switching and connects menu, game, and networking
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showMainMenu();
    void startSinglePlayer();
    void showHighScores();

    void startHostMultiplayer();
    void startJoinMultiplayer();

    void showNetworkStatus(const QString &message);
    void showNetworkMessage(const QString &message);

    void handleCountdown(int number);
    void startMultiplayerGame();

private:
    QStackedWidget *screenStack;

    MainMenuWidget *mainMenuWidget;
    HighScoresWidget *highScoresWidget;
    GameWidget *gameWidget;

    HostNetworkManager *hostNetworkManager;
    ClientNetworkManager *clientNetworkManager;
    DiscoveryService *discoveryService;
};

#endif // MAINWINDOW_H
