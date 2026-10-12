#ifndef OBJECTIVE_H
#define OBJECTIVE_H

#include "gameobject.h"
#include "player.h"

// crystal is used internally for the dragon egg
enum class ObjectiveType
{
    Key,
    Crystal,
    FinishCup
};

class Objective : public GameObject
{
public:
    Objective();
    Objective(ObjectiveType type, const QPointF &position);

    void update(float deltaTime) override;
    void draw(QPainter &painter, const QPointF &cameraPosition) override;

    void activate(Player *player);
    void damage(int amount);
    void forceComplete(); // used for multiplayer sync

    ObjectiveType getType() const;
    bool isCompleted() const;
    int getHealth() const;

private:
    ObjectiveType type;
    bool completed;
    int health;
};

#endif // OBJECTIVE_H
