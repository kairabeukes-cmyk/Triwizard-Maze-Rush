#include "enemy.h"

#include <QColor>
#include <QFont>
#include <QtMath>

Enemy::Enemy()
    : GameObject(QPointF(0, 0), QSizeF(40, 40)),
      enemyName("Enemy"),
      health(50),
      damage(10),
      speed(80.0f),
      currentPatrolIndex(0)
{
}

Enemy::Enemy(const QPointF &position,
             const QSizeF &size,
             const QString &enemyName,
             int health,
             int damage,
             float speed)
    : GameObject(position, size),
      enemyName(enemyName),
      health(health),
      damage(damage),
      speed(speed),
      currentPatrolIndex(0)
{
}

Enemy::~Enemy()
{
}

void Enemy::update(float deltaTime)
{
    Q_UNUSED(deltaTime);
}

void Enemy::update(float deltaTime, Player *player)
{
    Q_UNUSED(deltaTime);
    Q_UNUSED(player);
}

void Enemy::draw(QPainter &painter, const QPointF &cameraPosition)
{
    if (!active)
        return;

    QRectF screenRect(position.x() - cameraPosition.x(),
                      position.y() - cameraPosition.y(),
                      size.width(),
                      size.height());

    painter.setBrush(QColor(160, 80, 80));
    painter.setPen(QColor(255, 210, 210));
    painter.drawRoundedRect(screenRect, 6, 6);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8, QFont::Bold));
    painter.drawText(QRectF(screenRect.left() - 20,
                            screenRect.top() - 18,
                            screenRect.width() + 40,
                            15),
                     Qt::AlignCenter,
                     enemyName);
}

void Enemy::attack(Player *player)
{
    if (!player)
        return;

    player->takeDamage(damage);
}

void Enemy::takeDamage(int amount)
{
    if (amount <= 0)
        return;

    health -= amount;

    if (health <= 0)
    {
        health = 0;
        active = false;
    }
}

bool Enemy::isDead() const
{
    return health <= 0;
}

QString Enemy::getEnemyName() const
{
    return enemyName;
}

int Enemy::getHealth() const
{
    return health;
}

int Enemy::getDamage() const
{
    return damage;
}

QPointF Enemy::normalizedDirectionTo(const QPointF &target) const
{
    QPointF direction(target.x() - position.x(),
                      target.y() - position.y());

    double length = qSqrt(direction.x() * direction.x() +
                          direction.y() * direction.y());

    if (length <= 0.0)
        return QPointF(0, 0);

    direction.rx() /= length;
    direction.ry() /= length;

    return direction;
}

void Enemy::moveTowards(const QPointF &target, float deltaTime)
{
    QPointF direction = normalizedDirectionTo(target);

    velocity = QPointF(direction.x() * speed,
                       direction.y() * speed);

    position += velocity * deltaTime;
}
