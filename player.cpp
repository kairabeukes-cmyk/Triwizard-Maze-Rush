#include "player.h"

#include <QColor>
#include <QFont>
#include <QtMath>

Player::Player()
    : GameObject(QPointF(100, 100), QSizeF(42, 42)),
      name("Player"),
      health(100),
      baseSpeed(180.0f),
      terrainSpeedMultiplier(1.0f),
      powerupSpeedMultiplier(1.0f),
      movingUp(false),
      movingDown(false),
      movingLeft(false),
      movingRight(false),
      hasKey(false),
      finished(false),
      damageFlashTimer(0.0f),
      speedBoostTimer(0.0f),
      shieldTimer(0.0f)
{
}

Player::Player(const QString &name, const QPointF &startPosition)
    : GameObject(startPosition, QSizeF(42, 42)),
      name(name),
      health(100),
      baseSpeed(180.0f),
      terrainSpeedMultiplier(1.0f),
      powerupSpeedMultiplier(1.0f),
      movingUp(false),
      movingDown(false),
      movingLeft(false),
      movingRight(false),
      hasKey(false),
      finished(false),
      damageFlashTimer(0.0f),
      speedBoostTimer(0.0f),
      shieldTimer(0.0f)
{
}

void Player::update(float deltaTime)
{
    if (finished)
    {
        velocity = QPointF(0, 0);
        return;
    }

    if (damageFlashTimer > 0.0f)
    {
        damageFlashTimer -= deltaTime;

        if (damageFlashTimer < 0.0f)
            damageFlashTimer = 0.0f;
    }

    if (speedBoostTimer > 0.0f)
    {
        speedBoostTimer -= deltaTime;

        if (speedBoostTimer <= 0.0f)
        {
            speedBoostTimer = 0.0f;
            powerupSpeedMultiplier = 1.0f;
        }
    }

    if (shieldTimer > 0.0f)
    {
        shieldTimer -= deltaTime;

        if (shieldTimer < 0.0f)
            shieldTimer = 0.0f;
    }

    QPointF direction(0.0, 0.0);

    if (movingUp)
        direction.ry() -= 1.0;

    if (movingDown)
        direction.ry() += 1.0;

    if (movingLeft)
        direction.rx() -= 1.0;

    if (movingRight)
        direction.rx() += 1.0;

    double length = qSqrt(direction.x() * direction.x() +
                          direction.y() * direction.y());

    if (length > 0.0)
    {
        direction.rx() /= length;
        direction.ry() /= length;
    }

    float currentSpeed = baseSpeed * terrainSpeedMultiplier * powerupSpeedMultiplier;

    velocity = QPointF(direction.x() * currentSpeed,
                       direction.y() * currentSpeed);

    position += velocity * deltaTime;
}

void Player::draw(QPainter &painter, const QPointF &cameraPosition)
{
    QRectF screenRect(position.x() - cameraPosition.x(),
                      position.y() - cameraPosition.y(),
                      size.width(),
                      size.height());

    if (hasShield())
    {
        QRectF shieldRect = screenRect.adjusted(-7, -7, 7, 7);
        painter.setBrush(QColor(90, 130, 255, 80));
        painter.setPen(QColor(150, 190, 255));
        painter.drawEllipse(shieldRect);
    }

    QColor bodyColor(80, 130, 230);

    if (damageFlashTimer > 0.0f)
        bodyColor = QColor(230, 70, 70);

    if (hasSpeedBoost())
        bodyColor = QColor(80, 210, 255);

    if (finished)
        bodyColor = QColor(230, 200, 70);

    painter.setBrush(bodyColor);
    painter.setPen(QColor(220, 230, 255));
    painter.drawRoundedRect(screenRect, 8, 8);

    painter.setBrush(QColor(255, 255, 255));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QRectF(screenRect.center().x() - 5,
                               screenRect.top() + 8,
                               10,
                               10));

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    painter.drawText(QRectF(screenRect.left() - 20,
                            screenRect.top() - 20,
                            screenRect.width() + 40,
                            16),
                     Qt::AlignCenter,
                     name);
}

void Player::setMovingUp(bool value)
{
    movingUp = value;
}

void Player::setMovingDown(bool value)
{
    movingDown = value;
}

void Player::setMovingLeft(bool value)
{
    movingLeft = value;
}

void Player::setMovingRight(bool value)
{
    movingRight = value;
}

QString Player::getName() const
{
    return name;
}

int Player::getHealth() const
{
    return health;
}

bool Player::isAlive() const
{
    return health > 0;
}

void Player::takeDamage(int amount)
{
    if (amount <= 0 || finished)
        return;

    if (hasShield())
        return;

    health -= amount;

    if (health < 0)
        health = 0;

    damageFlashTimer = 0.15f;
}

void Player::heal(int amount)
{
    if (amount <= 0)
        return;

    health += amount;

    if (health > 100)
        health = 100;
}

void Player::setSpeedMultiplier(float multiplier)
{
    terrainSpeedMultiplier = multiplier;
}

float Player::getSpeedMultiplier() const
{
    return terrainSpeedMultiplier * powerupSpeedMultiplier;
}

bool Player::isDamageFlashActive() const
{
    return damageFlashTimer > 0.0f;
}

void Player::collectKey()
{
    hasKey = true;
}

bool Player::ownsKey() const
{
    return hasKey;
}

void Player::finish()
{
    finished = true;
}

bool Player::isFinished() const
{
    return finished;
}

void Player::activateSpeedBoost(float duration)
{
    speedBoostTimer = duration;
    powerupSpeedMultiplier = 1.6f;
}

void Player::activateShield(float duration)
{
    shieldTimer = duration;
}

bool Player::hasSpeedBoost() const
{
    return speedBoostTimer > 0.0f;
}

bool Player::hasShield() const
{
    return shieldTimer > 0.0f;
}

float Player::getSpeedBoostTimer() const
{
    return speedBoostTimer;
}

float Player::getShieldTimer() const
{
    return shieldTimer;
}
