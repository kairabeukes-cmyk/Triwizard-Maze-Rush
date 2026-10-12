#include "tile.h"

Tile::Tile()
    : type(TileType::Stone),
      rect(0, 0, 64, 64)
{
}

Tile::Tile(TileType type, const QRectF &rect)
    : type(type),
      rect(rect)
{
}

TileType Tile::getType() const
{
    return type;
}

QRectF Tile::getRect() const
{
    return rect;
}

bool Tile::isSolid() const
{
    return type == TileType::Wall;
}

float Tile::speedMultiplier() const
{
    if (type == TileType::Swamp)
        return 0.5f;

    return 1.0f;
}

int Tile::damagePerSecond() const
{
    if (type == TileType::Fire)
        return 10;

    return 0;
}

QColor Tile::color() const
{
    switch (type)
    {
    case TileType::Stone:
        return QColor(90, 90, 105);

    case TileType::Wall:
        return QColor(35, 35, 45);

    case TileType::Swamp:
        return QColor(40, 110, 65);

    case TileType::Fire:
        return QColor(190, 65, 35);

    case TileType::Finish:
        return QColor(215, 180, 45);
    }

    return QColor(255, 0, 255);
}
