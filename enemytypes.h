#ifndef ENEMYTYPES_H
#define ENEMYTYPES_H

#include "enemy.h"

#include <QString>

// patrol enemy
class Troll : public Enemy
{
public:
    explicit Troll(const QPointF &position);

    void update(float deltaTime, Player *player) override;
    void draw(QPainter &painter, const QPointF &cameraPosition) override;

private:
    float waitTimer;
};

// wandering enemy
class Pixie : public Enemy
{
public:
    explicit Pixie(const QPointF &position);

    void update(float deltaTime, Player *player) override;
    void draw(QPainter &painter, const QPointF &cameraPosition) override;

private:
    float directionTimer;
    QPointF randomDirection;

    QPointF homePosition;
    float wanderRadius;

    void chooseNewDirection();
    void steerBackHome();
};

// chasing enemy
class Spider : public Enemy
{
public:
    explicit Spider(const QPointF &position);

    void update(float deltaTime, Player *player) override;
    void draw(QPainter &painter, const QPointF &cameraPosition) override;

private:
    float detectionRadius;
};

// final boss enemy
class Dragon : public Enemy
{
public:
    explicit Dragon(const QPointF &startPosition);

    void update(float deltaTime, Player *player) override;
    void draw(QPainter &painter, const QPointF &cameraPosition) override;

    void takeHit();

    bool isDefeated() const;
    int getHealth() const;
    int getMaxHealth() const;

    void setFacingDirection(const QString &direction);
    QString getFacingDirection() const;

private:
    int maxHealth;
    float hurtTimer;
    QString facingDirection;
};

#endif // ENEMYTYPES_H
