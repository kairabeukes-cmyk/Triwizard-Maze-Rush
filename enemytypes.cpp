#include "enemytypes.h"
#include "spritemanager.h"

#include <QColor>
#include <QFont>
#include <QRandomGenerator>
#include <QtMath>

// troll

Troll::Troll(const QPointF &position)
    : Enemy(position,
            QSizeF(58, 58),
            "Troll",
            100,
            20,
            70.0f),
      waitTimer(0.0f)
{
    // fixed patrol route
    patrolPoints.append(position);
    patrolPoints.append(QPointF(position.x() + 260, position.y()));
    patrolPoints.append(QPointF(position.x() + 260, position.y() + 160));
    patrolPoints.append(QPointF(position.x(), position.y() + 160));
}

void Troll::update(float deltaTime, Player *player)
{
    Q_UNUSED(player);

    if (!active || patrolPoints.isEmpty())
        return;

    QPointF target = patrolPoints[currentPatrolIndex];

    moveTowards(target, deltaTime);

    double dx = target.x() - position.x();
    double dy = target.y() - position.y();
    double distance = qSqrt(dx * dx + dy * dy);

    if (distance < 8.0)
    {
        currentPatrolIndex++;

        if (currentPatrolIndex >= patrolPoints.size())
            currentPatrolIndex = 0;
    }
}

void Troll::draw(QPainter &painter, const QPointF &cameraPosition)
{
    if (!active)
        return;

    QRectF screenRect(position.x() - cameraPosition.x(),
                      position.y() - cameraPosition.y(),
                      size.width(),
                      size.height());

    QString spriteName = "troll_down";

    double absX = qAbs(velocity.x());
    double absY = qAbs(velocity.y());

    if (absX > absY)
    {
        if (velocity.x() < 0)
            spriteName = "troll_left";
        else if (velocity.x() > 0)
            spriteName = "troll_right";
    }
    else
    {
        if (velocity.y() < 0)
            spriteName = "troll_up";
        else if (velocity.y() > 0)
            spriteName = "troll_down";
    }

    if (SpriteManager::drawSprite(painter, spriteName, screenRect))
        return;

    // fallback troll drawing
    painter.setBrush(QColor(120, 90, 60));
    painter.setPen(QColor(230, 200, 150));
    painter.drawRoundedRect(screenRect, 8, 8);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8, QFont::Bold));
    painter.drawText(QRectF(screenRect.left() - 20,
                            screenRect.top() - 18,
                            screenRect.width() + 40,
                            15),
                     Qt::AlignCenter,
                     "Troll");
}

// pixie

Pixie::Pixie(const QPointF &position)
    : Enemy(position,
            QSizeF(30, 30),
            "Pixie",
            30,
            5,
            155.0f),
      directionTimer(0.0f),
      randomDirection(1, 0),
      homePosition(position),
      wanderRadius(180.0f)
{
    chooseNewDirection();
}

void Pixie::update(float deltaTime, Player *player)
{
    Q_UNUSED(player);

    if (!active)
        return;

    directionTimer -= deltaTime;

    QPointF fromHome(position.x() - homePosition.x(),
                     position.y() - homePosition.y());

    double distanceFromHome = qSqrt(fromHome.x() * fromHome.x() +
                                    fromHome.y() * fromHome.y());

    if (distanceFromHome > wanderRadius)
    {
        steerBackHome();
    }
    else if (directionTimer <= 0.0f)
    {
        chooseNewDirection();
    }

    velocity = QPointF(randomDirection.x() * speed,
                       randomDirection.y() * speed);

    position += velocity * deltaTime;
}

void Pixie::draw(QPainter &painter, const QPointF &cameraPosition)
{
    if (!active)
        return;

    QRectF screenRect(position.x() - cameraPosition.x(),
                      position.y() - cameraPosition.y(),
                      size.width(),
                      size.height());

    QString spriteName = "pixie_down";

    if (velocity.y() < 0)
        spriteName = "pixie_up";
    else if (velocity.y() > 0)
        spriteName = "pixie_down";

    if (SpriteManager::drawSprite(painter, spriteName, screenRect))
        return;

    // fallback pixie drawing
    painter.setBrush(QColor(190, 110, 230));
    painter.setPen(QColor(245, 220, 255));
    painter.drawEllipse(screenRect);

    painter.setBrush(QColor(255, 240, 255));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QRectF(screenRect.center().x() - 5,
                               screenRect.center().y() - 5,
                               10,
                               10));

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8, QFont::Bold));
    painter.drawText(QRectF(screenRect.left() - 25,
                            screenRect.top() - 18,
                            screenRect.width() + 50,
                            15),
                     Qt::AlignCenter,
                     "Pixie");
}

void Pixie::chooseNewDirection()
{
    double angle = QRandomGenerator::global()->bounded(628) / 100.0;

    randomDirection = QPointF(qCos(angle), qSin(angle));

    directionTimer = 0.7f + QRandomGenerator::global()->bounded(100) / 100.0f;
}

void Pixie::steerBackHome()
{
    QPointF direction(homePosition.x() - position.x(),
                      homePosition.y() - position.y());

    double length = qSqrt(direction.x() * direction.x() +
                          direction.y() * direction.y());

    if (length <= 0.0)
    {
        chooseNewDirection();
        return;
    }

    direction.rx() /= length;
    direction.ry() /= length;

    randomDirection = direction;
    directionTimer = 0.5f;
}

// spider

Spider::Spider(const QPointF &position)
    : Enemy(position,
            QSizeF(46, 46),
            "Spider",
            60,
            12,
            115.0f),
      detectionRadius(420.0f)
{
}

void Spider::update(float deltaTime, Player *player)
{
    if (!active || !player)
        return;

    QPointF playerPosition = player->getPosition();

    double dx = playerPosition.x() - position.x();
    double dy = playerPosition.y() - position.y();
    double distance = qSqrt(dx * dx + dy * dy);

    if (distance <= detectionRadius)
    {
        moveTowards(playerPosition, deltaTime);
    }
    else
    {
        velocity = QPointF(0, 0);
    }
}

void Spider::draw(QPainter &painter, const QPointF &cameraPosition)
{
    if (!active)
        return;

    QRectF screenRect(position.x() - cameraPosition.x(),
                      position.y() - cameraPosition.y(),
                      size.width(),
                      size.height());

    QString spriteName = "spider_down";

    if (velocity.y() < 0)
        spriteName = "spider_up";
    else if (velocity.y() > 0)
        spriteName = "spider_down";

    if (SpriteManager::drawSprite(painter, spriteName, screenRect))
        return;

    // fallback spider drawing
    painter.setBrush(QColor(45, 45, 50));
    painter.setPen(QColor(180, 180, 190));
    painter.drawEllipse(screenRect);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8, QFont::Bold));
    painter.drawText(QRectF(screenRect.left() - 25,
                            screenRect.top() - 18,
                            screenRect.width() + 50,
                            15),
                     Qt::AlignCenter,
                     "Spider");
}

// dragon

Dragon::Dragon(const QPointF &startPosition)
    : Enemy(startPosition,
            QSizeF(128, 128),
            "Dragon",
            20,
            25,
            0.0f),
      maxHealth(20),
      hurtTimer(0.0f),
      facingDirection("down")
{
}

void Dragon::update(float deltaTime, Player *player)
{
    Q_UNUSED(player);

    velocity = QPointF(0, 0);

    if (hurtTimer > 0.0f)
    {
        hurtTimer -= deltaTime;

        if (hurtTimer < 0.0f)
            hurtTimer = 0.0f;
    }
}

void Dragon::draw(QPainter &painter, const QPointF &cameraPosition)
{
    if (!active)
        return;

    QRectF screenRect(position.x() - cameraPosition.x(),
                      position.y() - cameraPosition.y(),
                      size.width(),
                      size.height());

    QString spriteName = "dragon_" + facingDirection;

    if (isDefeated())
        spriteName = "dragon_defeated";
    else if (hurtTimer > 0.0f)
        spriteName = "dragon_hurt";

    bool spriteDrawn = SpriteManager::drawSprite(painter,
                                                  spriteName,
                                                  screenRect);

    if (!spriteDrawn)
    {
        // fallback dragon drawing
        if (isDefeated())
            painter.setBrush(QColor(60, 60, 60));
        else if (hurtTimer > 0.0f)
            painter.setBrush(QColor(200, 60, 60));
        else
            painter.setBrush(QColor(120, 40, 25));

        painter.setPen(QPen(Qt::black, 3));
        painter.drawEllipse(screenRect);
    }

    if (!isDefeated())
    {
        QRectF healthBack(screenRect.left(),
                          screenRect.top() - 14,
                          screenRect.width(),
                          8);

        QRectF healthFront = healthBack;

        healthFront.setWidth(healthBack.width() *
                             static_cast<double>(health) /
                             static_cast<double>(maxHealth));

        painter.setBrush(QColor(40, 40, 40));
        painter.setPen(Qt::NoPen);
        painter.drawRect(healthBack);

        painter.setBrush(QColor(180, 40, 30));
        painter.drawRect(healthFront);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 9, QFont::Bold));
        painter.drawText(QRectF(screenRect.left(),
                                screenRect.top() - 38,
                                screenRect.width(),
                                20),
                         Qt::AlignCenter,
                         QString("Dragon %1/%2").arg(health).arg(maxHealth));
    }
}

void Dragon::takeHit()
{
    if (isDefeated())
        return;

    health--;

    if (health < 0)
        health = 0;

    hurtTimer = 0.25f;

    if (health <= 0)
    {
        health = 0;
        active = true;
    }
}

bool Dragon::isDefeated() const
{
    return health <= 0;
}

int Dragon::getHealth() const
{
    return health;
}

int Dragon::getMaxHealth() const
{
    return maxHealth;
}

void Dragon::setFacingDirection(const QString &direction)
{
    if (direction == "up" ||
        direction == "down" ||
        direction == "left" ||
        direction == "right")
    {
        facingDirection = direction;
    }
}

QString Dragon::getFacingDirection() const
{
    return facingDirection;
}
