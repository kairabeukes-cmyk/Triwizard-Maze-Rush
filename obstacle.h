#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "gameobject.h"
#include "player.h"

enum class ObstacleType
{
    LockedGate
};

class Obstacle : public GameObject
{
public:
    Obstacle();
    Obstacle(ObstacleType type, const QPointF &position, const QSizeF &size);

    void update(float deltaTime) override;
    void draw(QPainter &painter, const QPointF &cameraPosition) override;

    void interact(Player *player);
    void forceOpen(); // used for multiplayer sync

    ObstacleType getType() const;
    bool isOpen() const;
    bool isSolid() const;

private:
    ObstacleType type;
    bool open;
};

#endif // OBSTACLE_H
