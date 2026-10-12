#ifndef ENEMY_H
#define ENEMY_H

#include "gameobject.h"
#include "player.h"

#include <QVector>
#include <QPointF>
#include <QString>

// base class for all enemies
class Enemy : public GameObject
{
public:
    Enemy();

    Enemy(const QPointF &position,
          const QSizeF &size,
          const QString &enemyName,
          int health,
          int damage,
          float speed);

    virtual ~Enemy();

    void update(float deltaTime) override;
    virtual void update(float deltaTime, Player *player);

    virtual void draw(QPainter &painter, const QPointF &cameraPosition) override;

    virtual void attack(Player *player);

    void takeDamage(int amount);
    bool isDead() const;

    QString getEnemyName() const;
    int getHealth() const;
    int getDamage() const;

protected:
    QString enemyName;
    int health;
    int damage;
    float speed;

    QVector<QPointF> patrolPoints;
    int currentPatrolIndex;

    QPointF normalizedDirectionTo(const QPointF &target) const;
    void moveTowards(const QPointF &target, float deltaTime);
};

#endif // ENEMY_H
