#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"

#include <QString>

// stores player movement, health, objectives, and powerups
class Player : public GameObject
{
public:
    Player();
    Player(const QString &name, const QPointF &startPosition);

    void update(float deltaTime) override;
    void draw(QPainter &painter, const QPointF &cameraPosition) override;

    void setMovingUp(bool value);
    void setMovingDown(bool value);
    void setMovingLeft(bool value);
    void setMovingRight(bool value);

    QString getName() const;

    int getHealth() const;
    bool isAlive() const;

    void takeDamage(int amount);
    void heal(int amount);

    void setSpeedMultiplier(float multiplier);
    float getSpeedMultiplier() const;

    bool isDamageFlashActive() const;

    void collectKey();
    bool ownsKey() const;

    void finish();
    bool isFinished() const;

    void activateSpeedBoost(float duration);
    void activateShield(float duration);

    bool hasSpeedBoost() const;
    bool hasShield() const;

    float getSpeedBoostTimer() const;
    float getShieldTimer() const;

private:
    QString name;
    int health;

    float baseSpeed;
    float terrainSpeedMultiplier;
    float powerupSpeedMultiplier;

    bool movingUp;
    bool movingDown;
    bool movingLeft;
    bool movingRight;

    bool hasKey;
    bool finished;

    float damageFlashTimer;
    float speedBoostTimer;
    float shieldTimer;
};

#endif // PLAYER_H
