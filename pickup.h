#ifndef PICKUP_H
#define PICKUP_H

#include "gameobject.h"
#include "player.h"

enum class PickupType
{
    SpeedBoost,
    Shield,
    HealthPotion
};

class Pickup : public GameObject
{
public:
    Pickup();
    Pickup(PickupType type, const QPointF &position);

    void update(float deltaTime) override;
    void draw(QPainter &painter, const QPointF &cameraPosition) override;

    void applyTo(Player *player);

    PickupType getType() const;
    QString getName() const;

private:
    PickupType type;
};

#endif // PICKUP_H
