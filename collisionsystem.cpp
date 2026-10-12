#include "collisionsystem.h"

CollisionSystem::CollisionSystem()
{
}

bool CollisionSystem::playerCollidesWithSolidTile(const Player &player, const TileMap &tileMap) const
{
    return rectCollidesWithSolidTile(player.boundingBox(), tileMap);
}

bool CollisionSystem::rectCollidesWithSolidTile(const QRectF &rect, const TileMap &tileMap) const
{
    return tileMap.rectCollidesWithSolidTile(rect);
}
