#include "gamewidget.h"
#include "spritemanager.h"
#include "soundmanager.h"

#include <QPainter>
#include <QFont>
#include <QColor>

GameWidget::GameWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      dragonBoss(nullptr),
      dragonHasSpawned(false),
      dragonDefeated(false),
      dragonDefeatFlashTimer(0.0f),
      dragonFireWarningActive(false),
      dragonFireActive(false),
      dragonFireCooldownTimer(3.0f),
      dragonFireWarningTimer(0.0f),
      dragonFireActiveTimer(0.0f),
      dragonFireDamageTimer(0.0f),
      dragonHitCooldownTimer(0.0f),
      dragonFireDirection("down"),
      gameTimer(nullptr),
      player("Champion", QPointF(96, 96)),
      playState(PlayState::Playing),
      multiplayerRole(MultiplayerRole::None),
      totalTime(0.0f),
      fireDamageTimer(0.0f),
      enemyDamageTimer(0.0f),
      dragonDamageTimer(0.0f),
      scoreSaved(false),
      countdownVisible(false),
      countdownNumber(0),
      remotePlayerVisible(false),
      remotePlayerPosition(96, 160),
      previousRemotePlayerPosition(96, 160),
      remotePlayerVelocity(0, 0),
      remoteFinished(false),
      remoteFinishTime(0.0f),
      showLeaderboardOverlay(false),
      showHelpOverlay(false),
      soundMuted(false)
{
    setFocusPolicy(Qt::StrongFocus);

    camera.setWorldSize(QSizeF(tileMap.getWidthPixels(),
                               tileMap.getHeightPixels()));

    resetGame();

    gameTimer = new QTimer(this);

    connect(gameTimer, &QTimer::timeout,
            this, &GameWidget::updateGame);

    elapsedTimer.start();

    gameTimer->start(16);
}

GameWidget::~GameWidget()
{
    clearEnemies();

    if (dragonBoss)
    {
        delete dragonBoss;
        dragonBoss = nullptr;
    }
}

void GameWidget::initializeGL()
{
}

void GameWidget::resizeGL(int width, int height)
{
    camera.setViewportSize(QSizeF(width, height));
}

void GameWidget::resetGame()
{
    clearEnemies();

    player = Player("Champion", QPointF(3 * 64, 2 * 64));

    setupObjectivesAndObstacles();
    setupEnemies();
    setupPickups();

    if (dragonBoss)
    {
        delete dragonBoss;
        dragonBoss = nullptr;
    }

    dragonHasSpawned = false;
    dragonDefeated = false;
    dragonDefeatFlashTimer = 0.0f;

    dragonFireWarningActive = false;
    dragonFireActive = false;
    dragonFireCooldownTimer = 3.0f;
    dragonFireWarningTimer = 0.0f;
    dragonFireActiveTimer = 0.0f;
    dragonFireDamageTimer = 0.0f;
    dragonHitCooldownTimer = 0.0f;
    dragonFireDirection = "down";

    totalTime = 0.0f;
    fireDamageTimer = 0.0f;
    enemyDamageTimer = 0.0f;
    dragonDamageTimer = 0.0f;
    scoreSaved = false;
    playState = PlayState::Playing;
    multiplayerRole = MultiplayerRole::None;

    countdownVisible = false;
    countdownNumber = 0;

    remotePlayerVisible = false;
    remotePlayerPosition = QPointF(96, 160);
    previousRemotePlayerPosition = remotePlayerPosition;
    remotePlayerVelocity = QPointF(0, 0);
    remoteFinished = false;
    remoteFinishTime = 0.0f;

    showLeaderboardOverlay = false;
    showHelpOverlay = false;
    soundMuted = false;
    SoundManager::setMuted(false);

    elapsedTimer.restart();

    camera.follow(player.getPosition(), player.getSize());

    setFocus();
    update();
}

void GameWidget::startMultiplayerRace(MultiplayerRole role)
{
    resetGame();

    multiplayerRole = role;

    if (role == MultiplayerRole::Host)
    {
        player.setPosition(QPointF(3 * 64, 2 * 64));
        remotePlayerPosition = QPointF(3 * 64, 3 * 64);
    }
    else if (role == MultiplayerRole::Client)
    {
        player.setPosition(QPointF(3 * 64, 3 * 64));
        remotePlayerPosition = QPointF(3 * 64, 2 * 64);
    }

    previousRemotePlayerPosition = remotePlayerPosition;
    remotePlayerVelocity = QPointF(0, 0);

    remotePlayerVisible = (role != MultiplayerRole::None);
    remoteFinished = false;
    remoteFinishTime = 0.0f;

    countdownVisible = false;
    countdownNumber = 0;
    playState = PlayState::Playing;

    elapsedTimer.restart();

    camera.follow(player.getPosition(), player.getSize());

    setFocus();
}

void GameWidget::setCountdownDisplay(int number)
{
    countdownVisible = true;
    countdownNumber = number;
    update();
}

QPointF GameWidget::getLocalPlayerPosition() const
{
    return player.getPosition();
}

void GameWidget::setRemotePlayerPosition(const QPointF &position)
{
    previousRemotePlayerPosition = remotePlayerPosition;
    remotePlayerPosition = position;

    remotePlayerVelocity = QPointF(remotePlayerPosition.x() - previousRemotePlayerPosition.x(),
                                   remotePlayerPosition.y() - previousRemotePlayerPosition.y());

    remotePlayerVisible = true;
    update();
}

void GameWidget::playSoundForLocalEvent(const QString &eventName)
{
    if (eventName == "KEY_COLLECTED")
    {
        SoundManager::playSound("key");
    }
    else if (eventName == "GATE_OPENED")
    {
        SoundManager::playSound("gate");
    }
    else if (eventName == "CRYSTAL_HIT")
    {
        SoundManager::playSound("crystal");
    }
    else if (eventName == "FINISHED")
    {
        SoundManager::playSound("finish");
    }
    else if (eventName == "POWERUP")
    {
        SoundManager::playSound("powerup");
    }
    else if (eventName == "DRAGON_HIT")
    {
        SoundManager::playSound("crystal");
    }
}

void GameWidget::playDamageSoundIfHealthChanged(int oldHealth)
{
    if (player.getHealth() < oldHealth)
        SoundManager::playSound("damage");
}

void GameWidget::applyRemoteGameEvent(const QString &eventName)
{
    if (eventName == "KEY_COLLECTED")
    {
        applyKeyCollectedEvent();
        playSoundForLocalEvent("KEY_COLLECTED");
    }
    else if (eventName == "GATE_OPENED")
    {
        applyGateOpenedEvent();
        playSoundForLocalEvent("GATE_OPENED");
    }
    else if (eventName == "CRYSTAL_HIT")
    {
        applyCrystalHitEvent();
        playSoundForLocalEvent("CRYSTAL_HIT");
    }
    else if (eventName == "DRAGON_HIT")
    {
        applyDragonHitEvent();
        playSoundForLocalEvent("DRAGON_HIT");
    }
    else if (eventName == "DRAGON_DEFEATED")
    {
        applyDragonDefeatedEvent();
    }
    else if (eventName == "DRAGON_FIRE_WARNING_UP")
    {
        applyDragonFireWarningEvent("up");
    }
    else if (eventName == "DRAGON_FIRE_WARNING_DOWN")
    {
        applyDragonFireWarningEvent("down");
    }
    else if (eventName == "DRAGON_FIRE_WARNING_LEFT")
    {
        applyDragonFireWarningEvent("left");
    }
    else if (eventName == "DRAGON_FIRE_WARNING_RIGHT")
    {
        applyDragonFireWarningEvent("right");
    }
}

void GameWidget::setRemoteFinishTime(float finishTime)
{
    remoteFinished = true;
    remoteFinishTime = finishTime;
    update();
}

void GameWidget::clearEnemies()
{
    for (int i = 0; i < enemies.size(); ++i)
        delete enemies[i];

    enemies.clear();
}

void GameWidget::setupObjectivesAndObstacles()
{
    objectives.clear();
    obstacles.clear();

    objectives.append(Objective(ObjectiveType::Key,
                                QPointF(13 * 64, 2 * 64)));

    objectives.append(Objective(ObjectiveType::Crystal,
                                QPointF(28 * 64, 11 * 64)));

    objectives.append(Objective(ObjectiveType::FinishCup,
                                QPointF(38 * 64, 23 * 64)));

    obstacles.append(Obstacle(ObstacleType::LockedGate,
                              QPointF(22 * 64, 11 * 64),
                              QSizeF(64, 64)));
}

void GameWidget::setupEnemies()
{
    enemies.append(new Troll(QPointF(20 * 64, 3 * 64)));
    enemies.append(new Pixie(QPointF(35 * 64, 4 * 64)));
    enemies.append(new Spider(QPointF(15 * 64, 21 * 64)));
}

void GameWidget::setupPickups()
{
    pickups.clear();

    pickups.append(Pickup(PickupType::SpeedBoost,
                          QPointF(11 * 64, 6 * 64)));

    pickups.append(Pickup(PickupType::Shield,
                          QPointF(5 * 64, 14 * 64)));

    pickups.append(Pickup(PickupType::HealthPotion,
                          QPointF(23 * 64, 25 * 64)));
}

void GameWidget::setupDragonBoss()
{
    if (dragonBoss)
    {
        delete dragonBoss;
        dragonBoss = nullptr;
    }

    // dragon spawn position
    dragonBoss = new Dragon(QPointF(27 * 64, 22 * 64));

    dragonHasSpawned = true;
    dragonDefeated = false;

    dragonFireWarningActive = false;
    dragonFireActive = false;
    dragonFireCooldownTimer = 3.0f;
    dragonFireWarningTimer = 0.0f;
    dragonFireActiveTimer = 0.0f;
    dragonFireDamageTimer = 0.0f;
    dragonHitCooldownTimer = 0.0f;
    dragonFireDirection = "down";

    dragonBoss->setFacingDirection("down");
}

void GameWidget::updateDragonBoss(float deltaTime)
{
    if (!dragonHasSpawned && isDragonEggBroken())
        setupDragonBoss();

    if (dragonHitCooldownTimer > 0.0f)
    {
        dragonHitCooldownTimer -= deltaTime;

        if (dragonHitCooldownTimer < 0.0f)
            dragonHitCooldownTimer = 0.0f;
    }

    if (!dragonBoss || !dragonHasSpawned)
        return;

    dragonBoss->update(deltaTime, &player);

    if (dragonBoss->isDefeated())
    {
        dragonDefeated = true;
        return;
    }

    dragonDamageTimer += deltaTime;

    if (dragonBoss->boundingBox().intersects(player.boundingBox()))
    {
        if (dragonDamageTimer >= 0.9f)
        {
            int oldHealth = player.getHealth();

            player.takeDamage(5);

            playDamageSoundIfHealthChanged(oldHealth);

            dragonDamageTimer = 0.0f;
        }
    }
    else
    {
        if (dragonDamageTimer > 0.9f)
            dragonDamageTimer = 0.9f;
    }
}

void GameWidget::drawDragonBoss(QPainter &painter, const QPointF &cameraPosition)
{
    if (!dragonBoss || !dragonHasSpawned)
        return;

    dragonBoss->draw(painter, cameraPosition);
}

void GameWidget::handleDragonHit()
{
    if (!dragonBoss || !dragonHasSpawned || dragonDefeated)
        return;

    if (dragonHitCooldownTimer > 0.0f)
        return;

    QRectF hitRange = player.boundingBox().adjusted(-24, -24, 24, 24);

    if (!hitRange.intersects(dragonBoss->boundingBox()))
        return;

    dragonBoss->takeHit();

    playSoundForLocalEvent("DRAGON_HIT");

    if (multiplayerRole != MultiplayerRole::None)
        emit localGameEventGenerated("DRAGON_HIT");

    dragonHitCooldownTimer = 0.45f;

    if (dragonBoss->isDefeated())
    {
        dragonDefeated = true;
        dragonDefeatFlashTimer = 1.2f;

        stopDragonFire();

        if (multiplayerRole != MultiplayerRole::None)
            emit localGameEventGenerated("DRAGON_DEFEATED");
    }
}

bool GameWidget::isDragonEggBroken() const
{
    // the egg is stored as crystal internally
    for (int i = 0; i < objectives.size(); ++i)
    {
        if (objectives[i].getType() == ObjectiveType::Crystal)
            return objectives[i].isCompleted();
    }

    return false;
}

void GameWidget::updateDragonFire(float deltaTime)
{
    if (!dragonBoss || !dragonHasSpawned || dragonDefeated)
    {
        stopDragonFire();
        return;
    }

    if (dragonFireActive)
    {
        dragonFireActiveTimer -= deltaTime;
        dragonFireDamageTimer += deltaTime;

        if (dragonFireDamageTimer >= 0.4f)
        {
            dragonFireDamageTimer = 0.0f;

            QRectF fireRect = dragonFireWorldRect();

            if (fireRect.intersects(player.boundingBox()))
            {
                int oldHealth = player.getHealth();

                player.takeDamage(10);

                playDamageSoundIfHealthChanged(oldHealth);
            }
        }

        if (dragonFireActiveTimer <= 0.0f)
        {
            stopDragonFire();
            dragonFireCooldownTimer = 5.0f;
        }

        return;
    }

    if (dragonFireWarningActive)
    {
        dragonFireWarningTimer -= deltaTime;

        if (dragonFireWarningTimer <= 0.0f)
            activateDragonFire();

        return;
    }

    dragonFireCooldownTimer -= deltaTime;

    if (dragonFireCooldownTimer <= 0.0f)
    {
        // only the host chooses fire direction in multiplayer
        if (multiplayerRole == MultiplayerRole::None ||
            multiplayerRole == MultiplayerRole::Host)
        {
            startDragonFireWarning();
        }
    }
}

void GameWidget::startDragonFireWarning()
{
    if (!dragonBoss || dragonDefeated)
        return;

    dragonFireDirection = dragonDirectionTowardPlayer();

    dragonBoss->setFacingDirection(dragonFireDirection);

    dragonFireWarningActive = true;
    dragonFireActive = false;

    dragonFireWarningTimer = 1.25f;
    dragonFireActiveTimer = 0.0f;
    dragonFireDamageTimer = 0.0f;

    if (multiplayerRole != MultiplayerRole::None)
    {
        if (dragonFireDirection == "up")
            emit localGameEventGenerated("DRAGON_FIRE_WARNING_UP");
        else if (dragonFireDirection == "down")
            emit localGameEventGenerated("DRAGON_FIRE_WARNING_DOWN");
        else if (dragonFireDirection == "left")
            emit localGameEventGenerated("DRAGON_FIRE_WARNING_LEFT");
        else if (dragonFireDirection == "right")
            emit localGameEventGenerated("DRAGON_FIRE_WARNING_RIGHT");
    }
}

void GameWidget::activateDragonFire()
{
    dragonFireWarningActive = false;
    dragonFireActive = true;

    dragonFireActiveTimer = 2.0f;
    dragonFireDamageTimer = 0.0f;
}

void GameWidget::stopDragonFire()
{
    dragonFireWarningActive = false;
    dragonFireActive = false;

    dragonFireWarningTimer = 0.0f;
    dragonFireActiveTimer = 0.0f;
    dragonFireDamageTimer = 0.0f;
}

QRectF GameWidget::dragonFireWorldRect() const
{
    if (!dragonBoss)
        return QRectF();

    QRectF dragonBox = dragonBoss->boundingBox();

    const double tile = 64.0;
    const double laneWidth = 2.0 * tile;
    const double laneLength = 6.0 * tile;

    if (dragonFireDirection == "up")
    {
        return QRectF(dragonBox.center().x() - laneWidth / 2.0,
                      dragonBox.top() - laneLength,
                      laneWidth,
                      laneLength);
    }

    if (dragonFireDirection == "down")
    {
        return QRectF(dragonBox.center().x() - laneWidth / 2.0,
                      dragonBox.bottom(),
                      laneWidth,
                      laneLength);
    }

    if (dragonFireDirection == "left")
    {
        return QRectF(dragonBox.left() - laneLength,
                      dragonBox.center().y() - laneWidth / 2.0,
                      laneLength,
                      laneWidth);
    }

    return QRectF(dragonBox.right(),
                  dragonBox.center().y() - laneWidth / 2.0,
                  laneLength,
                  laneWidth);
}

QString GameWidget::dragonDirectionTowardPlayer() const
{
    if (!dragonBoss)
        return "down";

    QPointF dragonCenter = dragonBoss->boundingBox().center();
    QPointF playerCenter = player.boundingBox().center();

    double dx = playerCenter.x() - dragonCenter.x();
    double dy = playerCenter.y() - dragonCenter.y();

    if (qAbs(dx) > qAbs(dy))
    {
        if (dx < 0)
            return "left";

        return "right";
    }

    if (dy < 0)
        return "up";

    return "down";
}

void GameWidget::drawDragonFire(QPainter &painter, const QPointF &cameraPosition)
{
    if (!dragonFireWarningActive && !dragonFireActive)
        return;

    QRectF worldRect = dragonFireWorldRect();

    QRectF screenRect(worldRect.x() - cameraPosition.x(),
                      worldRect.y() - cameraPosition.y(),
                      worldRect.width(),
                      worldRect.height());

    if (dragonFireWarningActive)
    {
        // warning zone before fire damage starts
        painter.setBrush(QColor(255, 160, 40, 85));
        painter.setPen(QPen(QColor(255, 230, 120), 3, Qt::DashLine));
        painter.drawRect(screenRect);

        painter.setPen(QColor(255, 245, 190));
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(screenRect, Qt::AlignCenter, "FIRE!");

        return;
    }

    QString fireSprite = "dragon_fire";

    const int tile = 64;

    int startX = static_cast<int>(worldRect.left());
    int startY = static_cast<int>(worldRect.top());
    int endX = static_cast<int>(worldRect.right());
    int endY = static_cast<int>(worldRect.bottom());

    for (int y = startY; y < endY; y += tile)
    {
        for (int x = startX; x < endX; x += tile)
        {
            QRectF tileWorldRect(x, y, tile, tile);

            QRectF tileScreenRect(tileWorldRect.x() - cameraPosition.x(),
                                  tileWorldRect.y() - cameraPosition.y(),
                                  tileWorldRect.width(),
                                  tileWorldRect.height());

            bool spriteDrawn = SpriteManager::drawSprite(painter,
                                                          fireSprite,
                                                          tileScreenRect);

            if (!spriteDrawn)
            {
                painter.setBrush(QColor(240, 80, 25, 180));
                painter.setPen(QColor(255, 210, 70));
                painter.drawRect(tileScreenRect);
            }
        }
    }
}

void GameWidget::updateGame()
{
    float deltaTime = elapsedTimer.restart() / 1000.0f;

    if (playState != PlayState::Playing)
    {
        update();
        return;
    }

    totalTime += deltaTime;

    if (dragonDefeatFlashTimer > 0.0f)
    {
        dragonDefeatFlashTimer -= deltaTime;

        if (dragonDefeatFlashTimer < 0.0f)
            dragonDefeatFlashTimer = 0.0f;
    }

    applyTerrainEffects(deltaTime);

    QPointF oldPosition = player.getPosition();

    player.update(deltaTime);

    if (collisionSystem.playerCollidesWithSolidTile(player, tileMap) ||
        playerCollidesWithClosedObstacle())
    {
        player.setPosition(oldPosition);
    }

    updateObjectives();
    updateObstacles();
    updateEnemies(deltaTime);
    updatePickups(deltaTime);
    updateDragonBoss(deltaTime);
    updateDragonFire(deltaTime);
    handleEnemyPlayerDamage(deltaTime);

    if (!player.isAlive())
        handlePlayerDeath();

    if (player.isFinished())
        handlePlayerFinished();

    camera.follow(player.getPosition(), player.getSize());

    if (multiplayerRole != MultiplayerRole::None)
        emit localPlayerPositionChanged(player.getPosition());

    update();
}

void GameWidget::applyTerrainEffects(float deltaTime)
{
    QRectF playerBox = player.boundingBox();
    QPointF playerCenter = playerBox.center();

    TileType currentTile = tileMap.getTileTypeAtPixel(playerCenter);

    if (currentTile == TileType::Swamp)
        player.setSpeedMultiplier(0.5f);
    else
        player.setSpeedMultiplier(1.0f);

    if (currentTile == TileType::Fire)
    {
        fireDamageTimer += deltaTime;

        if (fireDamageTimer >= 0.5f)
        {
            int oldHealth = player.getHealth();

            player.takeDamage(5);

            playDamageSoundIfHealthChanged(oldHealth);

            fireDamageTimer = 0.0f;
        }
    }
    else
    {
        fireDamageTimer = 0.0f;
    }
}

void GameWidget::applyKeyCollectedEvent()
{
    for (int i = 0; i < objectives.size(); ++i)
    {
        if (objectives[i].getType() == ObjectiveType::Key)
            objectives[i].forceComplete();
    }
}

void GameWidget::applyGateOpenedEvent()
{
    for (int i = 0; i < obstacles.size(); ++i)
    {
        if (obstacles[i].getType() == ObstacleType::LockedGate)
            obstacles[i].forceOpen();
    }
}

void GameWidget::applyCrystalHitEvent()
{
    for (int i = 0; i < objectives.size(); ++i)
    {
        if (objectives[i].getType() == ObjectiveType::Crystal)
        {
            if (!objectives[i].isCompleted())
                objectives[i].damage(1);
        }
    }
}

void GameWidget::applyDragonHitEvent()
{
    // apply dragon damage from the remote player
    if (!dragonHasSpawned && isDragonEggBroken())
        setupDragonBoss();

    if (!dragonBoss || dragonDefeated)
        return;

    dragonBoss->takeHit();

    if (dragonBoss->isDefeated())
    {
        dragonDefeated = true;
        dragonDefeatFlashTimer = 1.2f;

        stopDragonFire();
    }
}

void GameWidget::applyDragonDefeatedEvent()
{
    // force local dragon defeat after remote final hit
    if (!dragonHasSpawned && isDragonEggBroken())
        setupDragonBoss();

    if (!dragonBoss)
        return;

    while (!dragonBoss->isDefeated())
        dragonBoss->takeHit();

    dragonDefeated = true;
    dragonDefeatFlashTimer = 1.2f;

    stopDragonFire();
}

void GameWidget::applyDragonFireWarningEvent(const QString &direction)
{
    // copy the remote fire direction locally
    if (!dragonBoss || !dragonHasSpawned || dragonDefeated)
        return;

    dragonFireDirection = direction;

    if (dragonBoss)
        dragonBoss->setFacingDirection(dragonFireDirection);

    dragonFireWarningActive = true;
    dragonFireActive = false;

    dragonFireWarningTimer = 1.25f;
    dragonFireActiveTimer = 0.0f;
    dragonFireDamageTimer = 0.0f;
    dragonFireCooldownTimer = 5.0f;
}

void GameWidget::drawDragonDefeatFlash(QPainter &painter)
{
    if (dragonDefeatFlashTimer <= 0.0f)
        return;

    double alphaRatio = dragonDefeatFlashTimer / 1.2;

    if (alphaRatio < 0.0)
        alphaRatio = 0.0;

    if (alphaRatio > 1.0)
        alphaRatio = 1.0;

    int alpha = static_cast<int>(140 * alphaRatio);

    painter.setBrush(QColor(255, 230, 120, alpha));
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    painter.setPen(QColor(255, 250, 210, static_cast<int>(255 * alphaRatio)));
    painter.setFont(QFont("Arial", 26, QFont::Bold));

    painter.drawText(QRectF(0,
                            height() / 2 - 80,
                            width(),
                            60),
                     Qt::AlignCenter,
                     "Dragon defeated!");
}

void GameWidget::updateObjectives()
{
    for (int i = 0; i < objectives.size(); ++i)
    {
        if (!objectives[i].isActive())
            continue;

        if (objectives[i].getType() == ObjectiveType::Key)
        {
            if (objectives[i].boundingBox().intersects(player.boundingBox()))
            {
                objectives[i].activate(&player);

                playSoundForLocalEvent("KEY_COLLECTED");

                if (multiplayerRole != MultiplayerRole::None)
                    emit localGameEventGenerated("KEY_COLLECTED");
            }
        }

        if (objectives[i].getType() == ObjectiveType::FinishCup)
        {
            if (objectives[i].boundingBox().intersects(player.boundingBox()))
            {
                if (dragonDefeated)
                    objectives[i].activate(&player);
            }
        }
    }
}

void GameWidget::updateObstacles()
{
    for (int i = 0; i < obstacles.size(); ++i)
        obstacles[i].update(0.0f);
}

void GameWidget::updateEnemies(float deltaTime)
{
    for (int i = 0; i < enemies.size(); ++i)
    {
        if (!enemies[i] || !enemies[i]->isActive())
            continue;

        QPointF oldPosition = enemies[i]->getPosition();

        enemies[i]->update(deltaTime, &player);

        // stop enemies from moving through walls
        if (tileMap.rectCollidesWithSolidTile(enemies[i]->boundingBox()))
        {
            enemies[i]->setPosition(oldPosition);
            enemies[i]->setVelocity(QPointF(0, 0));
        }
    }
}

void GameWidget::updatePickups(float deltaTime)
{
    for (int i = 0; i < pickups.size(); ++i)
    {
        pickups[i].update(deltaTime);

        if (!pickups[i].isActive())
            continue;

        if (pickups[i].boundingBox().intersects(player.boundingBox()))
        {
            pickups[i].applyTo(&player);
            playSoundForLocalEvent("POWERUP");
        }
    }
}

void GameWidget::handleInteraction()
{
    if (playState != PlayState::Playing)
        return;

    QRectF interactionBox = player.boundingBox().adjusted(-20, -20, 20, 20);

    for (int i = 0; i < obstacles.size(); ++i)
    {
        if (interactionBox.intersects(obstacles[i].boundingBox()))
        {
            bool wasOpen = obstacles[i].isOpen();

            obstacles[i].interact(&player);

            if (!wasOpen && obstacles[i].isOpen())
            {
                playSoundForLocalEvent("GATE_OPENED");

                if (multiplayerRole != MultiplayerRole::None)
                    emit localGameEventGenerated("GATE_OPENED");
            }
        }
    }
}

void GameWidget::handleCrystalAction()
{
    if (playState != PlayState::Playing)
        return;

    QRectF actionBox = player.boundingBox().adjusted(-35, -35, 35, 35);

    for (int i = 0; i < objectives.size(); ++i)
    {
        if (objectives[i].getType() == ObjectiveType::Crystal)
        {
            if (!objectives[i].isCompleted() &&
                actionBox.intersects(objectives[i].boundingBox()))
            {
                objectives[i].damage(1);

                playSoundForLocalEvent("CRYSTAL_HIT");

                if (multiplayerRole != MultiplayerRole::None)
                    emit localGameEventGenerated("CRYSTAL_HIT");
            }
        }
    }
}

void GameWidget::handleEnemyPlayerDamage(float deltaTime)
{
    enemyDamageTimer += deltaTime;

    if (enemyDamageTimer < 0.7f)
        return;

    for (int i = 0; i < enemies.size(); ++i)
    {
        if (!enemies[i] || !enemies[i]->isActive())
            continue;

        if (enemies[i]->boundingBox().intersects(player.boundingBox()))
        {
            int oldHealth = player.getHealth();

            enemies[i]->attack(&player);

            playDamageSoundIfHealthChanged(oldHealth);

            enemyDamageTimer = 0.0f;
            return;
        }
    }
}

bool GameWidget::playerCollidesWithClosedObstacle() const
{
    for (int i = 0; i < obstacles.size(); ++i)
    {
        if (obstacles[i].isSolid() &&
            obstacles[i].boundingBox().intersects(player.boundingBox()))
        {
            return true;
        }
    }

    return false;
}

bool GameWidget::crystalCompleted() const
{
    for (int i = 0; i < objectives.size(); ++i)
    {
        if (objectives[i].getType() == ObjectiveType::Crystal)
            return objectives[i].isCompleted();
    }

    return false;
}

void GameWidget::handlePlayerFinished()
{
    if (scoreSaved)
        return;

    leaderboard.addScore(player.getName(), totalTime);
    scoreSaved = true;
    playState = PlayState::Finished;

    playSoundForLocalEvent("FINISHED");

    if (multiplayerRole != MultiplayerRole::None)
        emit localPlayerFinished(totalTime);
}

void GameWidget::handlePlayerDeath()
{
    playState = PlayState::GameOver;
}

void GameWidget::paintGL()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), QColor(25, 25, 35));

    QPointF cameraPosition = camera.getPosition();

    tileMap.draw(painter, cameraPosition);

    for (int i = 0; i < obstacles.size(); ++i)
        obstacles[i].draw(painter, cameraPosition);

    for (int i = 0; i < pickups.size(); ++i)
        pickups[i].draw(painter, cameraPosition);

    for (int i = 0; i < objectives.size(); ++i)
        objectives[i].draw(painter, cameraPosition);

    for (int i = 0; i < enemies.size(); ++i)
    {
        if (enemies[i])
            enemies[i]->draw(painter, cameraPosition);
    }

    drawDragonBoss(painter, cameraPosition);
    drawDragonFire(painter, cameraPosition);

    drawRemotePlayer(painter, cameraPosition);
    drawLocalPlayer(painter, cameraPosition);

    drawControlsPanel(painter);
    drawStatusPanel(painter);
    drawRemoteFinishStatus(painter);

    drawDragonDefeatFlash(painter);

    if (showLeaderboardOverlay)
        drawLeaderboardOverlay(painter);

    if (showHelpOverlay)
        drawHelpOverlay(painter);

    if (countdownVisible)
        drawCountdownOverlay(painter);

    if (playState == PlayState::Finished)
        drawFinishOverlay(painter);

    if (playState == PlayState::GameOver)
        drawGameOverOverlay(painter);
}

void GameWidget::drawStatusPanel(QPainter &painter)
{
    QRectF box(width() - 285, 188, 265, 92);

    painter.setBrush(QColor(20, 20, 30, 175));
    painter.setPen(QColor(180, 180, 220));
    painter.drawRoundedRect(box, 8, 8);

    painter.setPen(QColor(250, 230, 130));
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(box.left() + 15,
                     box.top() + 24,
                     QString("Time: %1").arg(leaderboard.formatTime(totalTime)));

    int barX = static_cast<int>(box.left() + 15);
    int barY = static_cast<int>(box.top() + 42);
    int barWidth = 235;
    int barHeight = 18;

    QRectF backgroundRect(barX, barY, barWidth, barHeight);
    QRectF healthRect(barX,
                      barY,
                      barWidth * (player.getHealth() / 100.0),
                      barHeight);

    painter.setBrush(QColor(60, 60, 70));
    painter.setPen(QColor(210, 210, 230));
    painter.drawRect(backgroundRect);

    QColor healthColor(70, 200, 90);

    if (player.getHealth() <= 50)
        healthColor = QColor(220, 180, 50);

    if (player.getHealth() <= 25)
        healthColor = QColor(220, 70, 60);

    painter.setBrush(healthColor);
    painter.setPen(Qt::NoPen);
    painter.drawRect(healthRect);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    painter.drawText(backgroundRect,
                     Qt::AlignCenter,
                     QString("Health: %1").arg(player.getHealth()));

    QString soundText = soundMuted ? "Muted" : "Sound on";

    painter.setPen(QColor(210, 210, 220));
    painter.setFont(QFont("Arial", 8));
    painter.drawText(box.left() + 15,
                     box.top() + 79,
                     QString("H: Help | Tab: Scores | %1").arg(soundText));
}

void GameWidget::drawControlsPanel(QPainter &painter)
{
    QRectF box(width() - 285, 20, 265, 158);

    painter.setBrush(QColor(20, 20, 30, 175));
    painter.setPen(QColor(180, 180, 220));
    painter.drawRoundedRect(box, 8, 8);

    painter.setPen(QColor(250, 230, 130));
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(box.left() + 15, box.top() + 25, "Controls");

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 9));

    painter.drawText(box.left() + 15, box.top() + 50, "WASD / Arrows: Move");
    painter.drawText(box.left() + 15, box.top() + 70, "E: Open gate");
    painter.drawText(box.left() + 15, box.top() + 90, "Space: Hit egg/dragon");
    painter.drawText(box.left() + 15, box.top() + 110, "H: Help | Tab: Scores");
    painter.drawText(box.left() + 15, box.top() + 130, "R: Restart | N: Mute");
    painter.drawText(box.left() + 15, box.top() + 150, "Esc/M: Menu");
}

void GameWidget::drawLeaderboardOverlay(QPainter &painter)
{
    QRectF overlay(width() / 2 - 260,
                   height() / 2 - 210,
                   520,
                   420);

    painter.setBrush(QColor(20, 20, 30, 235));
    painter.setPen(QColor(250, 230, 130));
    painter.drawRoundedRect(overlay, 12, 12);

    painter.setPen(QColor(250, 230, 130));
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(QRectF(overlay.left(),
                            overlay.top() + 20,
                            overlay.width(),
                            40),
                     Qt::AlignCenter,
                     "Leaderboard");

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));

    QString localStatus = QString("You: %1").arg(leaderboard.formatTime(totalTime));

    if (playState == PlayState::Finished)
        localStatus += " [Finished]";
    else if (playState == PlayState::GameOver)
        localStatus += " [Game Over]";
    else
        localStatus += " [Racing]";

    painter.drawText(overlay.left() + 45,
                     overlay.top() + 85,
                     localStatus);

    QString remoteStatus = "Remote: not connected";

    if (multiplayerRole != MultiplayerRole::None)
    {
        if (remoteFinished)
        {
            remoteStatus = QString("Remote: %1 [Finished]")
                               .arg(leaderboard.formatTime(remoteFinishTime));
        }
        else
        {
            remoteStatus = "Remote: racing";
        }
    }

    painter.drawText(overlay.left() + 45,
                     overlay.top() + 115,
                     remoteStatus);

    painter.setPen(QColor(180, 180, 210));
    painter.drawLine(overlay.left() + 35,
                     overlay.top() + 145,
                     overlay.right() - 35,
                     overlay.top() + 145);

    painter.setPen(QColor(250, 230, 130));
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(QRectF(overlay.left(),
                            overlay.top() + 160,
                            overlay.width(),
                            30),
                     Qt::AlignCenter,
                     "Saved Best Times");

    QVector<ScoreEntry> topScores = leaderboard.getTopScores(8);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Consolas", 11));

    if (topScores.isEmpty())
    {
        painter.drawText(QRectF(overlay.left(),
                                overlay.top() + 215,
                                overlay.width(),
                                25),
                         Qt::AlignCenter,
                         "No saved scores yet");
    }
    else
    {
        for (int i = 0; i < topScores.size(); ++i)
        {
            QString line = QString("%1. %2 - %3")
                               .arg(i + 1)
                               .arg(topScores[i].playerName)
                               .arg(leaderboard.formatTime(topScores[i].timeSeconds));

            painter.drawText(overlay.left() + 80,
                             overlay.top() + 215 + i * 22,
                             line);
        }
    }

    painter.setPen(QColor(210, 210, 220));
    painter.setFont(QFont("Arial", 10));
    painter.drawText(QRectF(overlay.left(),
                            overlay.bottom() - 38,
                            overlay.width(),
                            25),
                     Qt::AlignCenter,
                     "Press Tab to close");
}

void GameWidget::drawHelpOverlay(QPainter &painter)
{
    QRectF overlay(width() / 2 - 430,
                   height() / 2 - 300,
                   860,
                   600);

    painter.setBrush(QColor(5, 5, 10, 190));
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    bool spriteDrawn = SpriteManager::drawSprite(painter,
                                                  "help_screen",
                                                  overlay);

    if (!spriteDrawn)
    {
        painter.setBrush(QColor(20, 20, 30, 245));
        painter.setPen(QColor(250, 230, 130));
        painter.drawRoundedRect(overlay, 14, 14);

        painter.setPen(QColor(250, 230, 130));
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(QRectF(overlay.left(),
                                overlay.top() + 25,
                                overlay.width(),
                                40),
                         Qt::AlignCenter,
                         "Help");

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 13));

        int x = static_cast<int>(overlay.left() + 55);
        int y = static_cast<int>(overlay.top() + 95);

        painter.drawText(x, y, "Goal:");
        painter.drawText(x + 20, y + 30, "1. Collect the key");
        painter.drawText(x + 20, y + 55, "2. Open the locked gate with E");
        painter.drawText(x + 20, y + 80, "3. Break the dragon egg with Space");
        painter.drawText(x + 20, y + 105, "4. Defeat the dragon and dodge fire");
        painter.drawText(x + 20, y + 130, "5. Reach the Triwizard Cup");

        painter.drawText(x, y + 190, "Controls:");
        painter.drawText(x + 20, y + 220, "WASD / Arrows - Move");
        painter.drawText(x + 20, y + 245, "E - Open gate");
        painter.drawText(x + 20, y + 270, "Space - Hit egg / dragon");
        painter.drawText(x + 20, y + 295, "Tab - Leaderboard");
        painter.drawText(x + 20, y + 320, "H - Close help");
        painter.drawText(x + 20, y + 345, "R - Restart");
        painter.drawText(x + 20, y + 370, "Esc / M - Menu");
    }

    painter.setPen(QColor(255, 245, 190));
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(QRectF(overlay.left(),
                            overlay.bottom() + 12,
                            overlay.width(),
                            25),
                     Qt::AlignCenter,
                     "Press H to close");
}

void GameWidget::drawMiniLeaderboard(QPainter &painter, const QRectF &area)
{
    painter.setBrush(QColor(35, 35, 48));
    painter.setPen(QColor(180, 180, 210));
    painter.drawRoundedRect(area, 8, 8);

    painter.setPen(QColor(250, 230, 130));
    painter.setFont(QFont("Arial", 13, QFont::Bold));
    painter.drawText(QRectF(area.left(),
                            area.top() + 10,
                            area.width(),
                            25),
                     Qt::AlignCenter,
                     "Best Times");

    QVector<ScoreEntry> topScores = leaderboard.getTopScores(5);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Consolas", 11));

    if (topScores.isEmpty())
    {
        painter.drawText(QRectF(area.left(),
                                area.top() + 55,
                                area.width(),
                                25),
                         Qt::AlignCenter,
                         "No scores yet");
        return;
    }

    for (int i = 0; i < topScores.size(); ++i)
    {
        QString line = QString("%1. %2 - %3")
                           .arg(i + 1)
                           .arg(topScores[i].playerName)
                           .arg(leaderboard.formatTime(topScores[i].timeSeconds));

        painter.drawText(area.left() + 30,
                         area.top() + 58 + i * 22,
                         line);
    }
}

QString GameWidget::multiplayerRoleText() const
{
    if (multiplayerRole == MultiplayerRole::Host)
        return "Role: Host";

    if (multiplayerRole == MultiplayerRole::Client)
        return "Role: Client";

    return "Single player";
}

void GameWidget::drawRemotePlayer(QPainter &painter, const QPointF &cameraPosition)
{
    if (!remotePlayerVisible)
        return;

    QRectF screenRect(remotePlayerPosition.x() - cameraPosition.x(),
                      remotePlayerPosition.y() - cameraPosition.y(),
                      42,
                      42);

    QString spriteName = remotePlayerSpriteName();

    if (SpriteManager::drawSprite(painter, spriteName, screenRect))
    {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 9, QFont::Bold));

        QString nameText = "Remote";

        if (characterPrefixForRemotePlayer() == "host")
            nameText = "Harry";
        else if (characterPrefixForRemotePlayer() == "client")
            nameText = "Fleur";

        painter.drawText(QRectF(screenRect.left() - 25,
                                screenRect.top() - 20,
                                screenRect.width() + 50,
                                16),
                         Qt::AlignCenter,
                         nameText);

        return;
    }

    // fallback remote player drawing
    painter.setBrush(QColor(230, 120, 80));
    painter.setPen(QColor(255, 230, 210));
    painter.drawRoundedRect(screenRect, 8, 8);

    painter.setBrush(QColor(255, 255, 255));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QRectF(screenRect.center().x() - 5,
                               screenRect.top() + 8,
                               10,
                               10));

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    painter.drawText(QRectF(screenRect.left() - 30,
                            screenRect.top() - 20,
                            screenRect.width() + 60,
                            16),
                     Qt::AlignCenter,
                     "Remote");
}

void GameWidget::drawLocalPlayer(QPainter &painter, const QPointF &cameraPosition)
{
    QRectF screenRect(player.getPosition().x() - cameraPosition.x(),
                      player.getPosition().y() - cameraPosition.y(),
                      player.getSize().width(),
                      player.getSize().height());

    QString spriteName = localPlayerSpriteName();

    if (SpriteManager::drawSprite(painter, spriteName, screenRect))
    {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 9, QFont::Bold));

        QString nameText = "Harry";

        if (characterPrefixForLocalPlayer() == "client")
            nameText = "Fleur";

        painter.drawText(QRectF(screenRect.left() - 25,
                                screenRect.top() - 20,
                                screenRect.width() + 50,
                                16),
                         Qt::AlignCenter,
                         nameText);

        return;
    }

    player.draw(painter, cameraPosition);
}

QString GameWidget::localPlayerSpriteName() const
{
    QString prefix = characterPrefixForLocalPlayer();

    if (player.isFinished())
        return prefix + "_win";

    if (player.isDamageFlashActive())
        return prefix + "_hurt";

    if (player.hasShield())
        return prefix + "_shield";

    if (player.ownsKey())
        return prefix + "_key";

    QString direction = movementDirectionFromVelocity(player.getVelocity());

    if (direction == "idle")
        return prefix + "_idle";

    return prefix + "_" + direction;
}

QString GameWidget::remotePlayerSpriteName() const
{
    QString prefix = characterPrefixForRemotePlayer();

    if (remoteFinished)
        return prefix + "_win";

    QString direction = movementDirectionFromVelocity(remotePlayerVelocity);

    if (direction == "idle")
        return prefix + "_idle";

    return prefix + "_" + direction;
}

QString GameWidget::movementDirectionFromVelocity(const QPointF &velocity) const
{
    double absX = qAbs(velocity.x());
    double absY = qAbs(velocity.y());

    if (absX < 0.01 && absY < 0.01)
        return "idle";

    if (absX > absY)
    {
        if (velocity.x() < 0)
            return "left";

        return "right";
    }

    if (velocity.y() < 0)
        return "up";

    return "down";
}

QString GameWidget::characterPrefixForLocalPlayer() const
{
    if (multiplayerRole == MultiplayerRole::Client)
        return "client";

    return "host";
}

QString GameWidget::characterPrefixForRemotePlayer() const
{
    if (multiplayerRole == MultiplayerRole::Host)
        return "client";

    if (multiplayerRole == MultiplayerRole::Client)
        return "host";

    return "client";
}

void GameWidget::drawRemoteFinishStatus(QPainter &painter)
{
    if (multiplayerRole == MultiplayerRole::None)
        return;

    QRectF box(20, 282, 300, 42);

    painter.setBrush(QColor(20, 20, 30, 170));
    painter.setPen(QColor(230, 170, 120));
    painter.drawRoundedRect(box, 8, 8);

    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.setPen(Qt::white);

    QString text = "Remote player: racing";

    if (remoteFinished)
    {
        text = QString("Remote finished: %1")
                   .arg(leaderboard.formatTime(remoteFinishTime));
    }

    painter.drawText(box, Qt::AlignCenter, text);
}

void GameWidget::drawObjectiveStatus(QPainter &painter)
{
    QRectF box(20, 110, 330, 112);

    painter.setBrush(QColor(20, 20, 30, 170));
    painter.setPen(QColor(220, 220, 240));
    painter.drawRoundedRect(box, 8, 8);

    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.setPen(Qt::white);

    painter.drawText(35, 135,
                     QString("1. Key collected: %1")
                         .arg(player.ownsKey() ? "yes" : "no"));

    painter.drawText(35, 160,
                     QString("2. Dragon egg broken: %1")
                         .arg(crystalCompleted() ? "yes" : "no"));

    painter.drawText(35, 185,
                     QString("3. Dragon defeated: %1")
                         .arg(dragonDefeated ? "yes" : "no"));

    painter.drawText(35, 210,
                     "4. Reach the Triwizard Cup");
}

void GameWidget::drawCountdownOverlay(QPainter &painter)
{
    QRectF overlay(width() / 2 - 150,
                   height() / 2 - 100,
                   300,
                   200);

    painter.setBrush(QColor(20, 20, 30, 220));
    painter.setPen(QColor(240, 220, 120));
    painter.drawRoundedRect(overlay, 12, 12);

    painter.setPen(QColor(250, 230, 130));
    painter.setFont(QFont("Arial", 52, QFont::Bold));

    QString text;

    if (countdownNumber > 0)
        text = QString::number(countdownNumber);
    else
        text = "GO!";

    painter.drawText(overlay, Qt::AlignCenter, text);
}

void GameWidget::drawPowerupStatus(QPainter &painter)
{
    QRectF box(20, 208, 300, 64);

    painter.setBrush(QColor(20, 20, 30, 170));
    painter.setPen(QColor(180, 200, 255));
    painter.drawRoundedRect(box, 8, 8);

    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.setPen(Qt::white);

    QString boostText = "Speed boost: inactive";
    QString shieldText = "Shield: inactive";

    if (player.hasSpeedBoost())
    {
        boostText = QString("Speed boost: %1 s")
                        .arg(player.getSpeedBoostTimer(), 0, 'f', 1);
    }

    if (player.hasShield())
    {
        shieldText = QString("Shield: %1 s")
                         .arg(player.getShieldTimer(), 0, 'f', 1);
    }

    painter.drawText(35, 233, boostText);
    painter.drawText(35, 258, shieldText);
}

void GameWidget::drawFinishOverlay(QPainter &painter)
{
    QRectF overlay(width() / 2 - 260,
                   height() / 2 - 180,
                   520,
                   360);

    painter.setBrush(QColor(20, 20, 30, 235));
    painter.setPen(QColor(240, 220, 120));
    painter.drawRoundedRect(overlay, 12, 12);

    painter.setPen(QColor(250, 230, 130));
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(QRectF(overlay.left(),
                            overlay.top() + 25,
                            overlay.width(),
                            45),
                     Qt::AlignCenter,
                     "Finished!");

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 15));
    painter.drawText(QRectF(overlay.left() + 30,
                            overlay.top() + 78,
                            overlay.width() - 60,
                            30),
                     Qt::AlignCenter,
                     "You defeated the dragon and reached the Triwizard Cup!");

    painter.drawText(QRectF(overlay.left(),
                            overlay.top() + 110,
                            overlay.width(),
                            35),
                     Qt::AlignCenter,
                     QString("Completion time: %1")
                         .arg(leaderboard.formatTime(totalTime)));

    QRectF leaderboardArea(overlay.left() + 55,
                           overlay.top() + 155,
                           overlay.width() - 110,
                           145);

    drawMiniLeaderboard(painter, leaderboardArea);

    painter.setFont(QFont("Arial", 10));
    painter.setPen(QColor(210, 210, 220));
    painter.drawText(QRectF(overlay.left(),
                            overlay.bottom() - 42,
                            overlay.width(),
                            25),
                     Qt::AlignCenter,
                     "Press R to restart or M/Esc for menu.");
}

void GameWidget::drawGameOverOverlay(QPainter &painter)
{
    QRectF overlay(width() / 2 - 230,
                   height() / 2 - 110,
                   460,
                   220);

    painter.setBrush(QColor(20, 20, 30, 235));
    painter.setPen(QColor(230, 90, 90));
    painter.drawRoundedRect(overlay, 12, 12);

    painter.setPen(QColor(240, 90, 90));
    painter.setFont(QFont("Arial", 26, QFont::Bold));
    painter.drawText(QRectF(overlay.left(),
                            overlay.top() + 35,
                            overlay.width(),
                            45),
                     Qt::AlignCenter,
                     "Game Over");

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(QRectF(overlay.left() + 30,
                            overlay.top() + 95,
                            overlay.width() - 60,
                            35),
                     Qt::AlignCenter,
                     "Your health reached zero.");

    painter.setFont(QFont("Arial", 11));
    painter.drawText(QRectF(overlay.left(),
                            overlay.top() + 150,
                            overlay.width(),
                            35),
                     Qt::AlignCenter,
                     "Press R to restart or M/Esc for menu.");
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        QOpenGLWidget::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_M)
    {
        emit returnToMenuRequested();
        return;
    }

    if (event->key() == Qt::Key_R)
    {
        resetGame();
        return;
    }

    if (event->key() == Qt::Key_Tab)
    {
        showLeaderboardOverlay = !showLeaderboardOverlay;

        if (showLeaderboardOverlay)
            showHelpOverlay = false;

        update();
        return;
    }

    if (event->key() == Qt::Key_H)
    {
        showHelpOverlay = !showHelpOverlay;

        if (showHelpOverlay)
            showLeaderboardOverlay = false;

        update();
        return;
    }

    if (event->key() == Qt::Key_N)
    {
        soundMuted = !soundMuted;
        SoundManager::setMuted(soundMuted);
        update();
        return;
    }

    if (playState != PlayState::Playing)
        return;

    if (event->key() == Qt::Key_W || event->key() == Qt::Key_Up)
        player.setMovingUp(true);

    if (event->key() == Qt::Key_S || event->key() == Qt::Key_Down)
        player.setMovingDown(true);

    if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left)
        player.setMovingLeft(true);

    if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right)
        player.setMovingRight(true);

    if (event->key() == Qt::Key_E)
        handleInteraction();

    if (event->key() == Qt::Key_Space)
    {
        handleCrystalAction();
        handleDragonHit();
    }
}

void GameWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        QOpenGLWidget::keyReleaseEvent(event);
        return;
    }

    if (event->key() == Qt::Key_W || event->key() == Qt::Key_Up)
        player.setMovingUp(false);

    if (event->key() == Qt::Key_S || event->key() == Qt::Key_Down)
        player.setMovingDown(false);

    if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left)
        player.setMovingLeft(false);

    if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right)
        player.setMovingRight(false);
}
