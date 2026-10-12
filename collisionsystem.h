#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include <QRectF>

#include "tilemap.h"
#include "player.h"

// checks player and object collisions with solid tiles
class CollisionSystem
{
public:
    CollisionSystem();

    bool playerCollidesWithSolidTile(const Player &player, const TileMap &tileMap) const;
    bool rectCollidesWithSolidTile(const QRectF &rect, const TileMap &tileMap) const;
};

#endif // COLLISIONSYSTEM_H
