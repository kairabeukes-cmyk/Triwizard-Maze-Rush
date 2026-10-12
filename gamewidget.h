#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QOpenGLWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QVector>

#include "tilemap.h"
#include "player.h"
#include "camera.h"
#include "collisionsystem.h"
#include "objective.h"
#include "obstacle.h"
#include "leaderboard.h"
#include "enemy.h"
#include "enemytypes.h"
#include "pickup.h"

// main game state
enum class PlayState
{
    Playing,
    Finished,
    GameOver
};

// current multiplayer role
enum class MultiplayerRole
{
    None,
    Host,
    Client
};

class GameWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget();

    void resetGame();
    void startMultiplayerRace(MultiplayerRole role = MultiplayerRole::None);
    void setCountdownDisplay(int number);

    QPointF getLocalPlayerPosition() const;
    void setRemotePlayerPosition(const QPointF &position);

    void applyRemoteGameEvent(const QString &eventName);
    void setRemoteFinishTime(float finishTime);

signals:
    void returnToMenuRequested();
    void localPlayerPositionChanged(const QPointF &position);
    void localGameEventGenerated(const QString &eventName);
    void localPlayerFinished(float finishTime);

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updateGame();

private:
    void setupObjectivesAndObstacles();
    void setupEnemies();
    void setupPickups();
    void clearEnemies();

    Dragon *dragonBoss;

    bool dragonHasSpawned;
    bool dragonDefeated;
    float dragonDefeatFlashTimer;

    bool dragonFireWarningActive;
    bool dragonFireActive;

    float dragonFireCooldownTimer;
    float dragonFireWarningTimer;
    float dragonFireActiveTimer;
    float dragonFireDamageTimer;
    float dragonHitCooldownTimer;

    QString dragonFireDirection;

    void setupDragonBoss();
    void updateDragonBoss(float deltaTime);
    void drawDragonBoss(QPainter &painter, const QPointF &cameraPosition);
    void handleDragonHit();
    bool isDragonEggBroken() const;

    void updateDragonFire(float deltaTime);
    void startDragonFireWarning();
    void activateDragonFire();
    void stopDragonFire();
    void drawDragonFire(QPainter &painter, const QPointF &cameraPosition);
    void drawDragonDefeatFlash(QPainter &painter);
    QRectF dragonFireWorldRect() const;
    QString dragonDirectionTowardPlayer() const;

    void applyTerrainEffects(float deltaTime);
    void updateObjectives();
    void updateObstacles();
    void updateEnemies(float deltaTime);
    void updatePickups(float deltaTime);

    void handleInteraction();
    void handleCrystalAction();
    void handleEnemyPlayerDamage(float deltaTime);

    bool playerCollidesWithClosedObstacle() const;
    bool crystalCompleted() const;

    void handlePlayerFinished();
    void handlePlayerDeath();

    void applyKeyCollectedEvent();
    void applyGateOpenedEvent();
    void applyCrystalHitEvent();

    void applyDragonHitEvent();
    void applyDragonDefeatedEvent();
    void applyDragonFireWarningEvent(const QString &direction);

    void playSoundForLocalEvent(const QString &eventName);
    void playDamageSoundIfHealthChanged(int oldHealth);

    void drawLocalPlayer(QPainter &painter, const QPointF &cameraPosition);
    void drawRemotePlayer(QPainter &painter, const QPointF &cameraPosition);

    QString localPlayerSpriteName() const;
    QString remotePlayerSpriteName() const;
    QString movementDirectionFromVelocity(const QPointF &velocity) const;
    QString characterPrefixForLocalPlayer() const;
    QString characterPrefixForRemotePlayer() const;

    void drawStatusPanel(QPainter &painter);
    void drawObjectiveStatus(QPainter &painter);
    void drawPowerupStatus(QPainter &painter);
    void drawFinishOverlay(QPainter &painter);
    void drawGameOverOverlay(QPainter &painter);
    void drawHelpOverlay(QPainter &painter);
    void drawMiniLeaderboard(QPainter &painter, const QRectF &area);
    void drawCountdownOverlay(QPainter &painter);
    void drawRemoteFinishStatus(QPainter &painter);
    void drawControlsPanel(QPainter &painter);
    void drawLeaderboardOverlay(QPainter &painter);
    QString multiplayerRoleText() const;

    QTimer *gameTimer;
    QElapsedTimer elapsedTimer;

    TileMap tileMap;
    Player player;
    Camera camera;
    CollisionSystem collisionSystem;

    QVector<Objective> objectives;
    QVector<Obstacle> obstacles;
    QVector<Enemy*> enemies;
    QVector<Pickup> pickups;

    Leaderboard leaderboard;

    PlayState playState;
    MultiplayerRole multiplayerRole;

    float totalTime;
    float fireDamageTimer;
    float enemyDamageTimer;
    float dragonDamageTimer;
    bool scoreSaved;

    bool countdownVisible;
    int countdownNumber;

    bool remotePlayerVisible;
    QPointF remotePlayerPosition;
    QPointF previousRemotePlayerPosition;
    QPointF remotePlayerVelocity;

    bool remoteFinished;
    float remoteFinishTime;

    bool showLeaderboardOverlay;
    bool showHelpOverlay;
    bool soundMuted;
};

#endif // GAMEWIDGET_H
