#ifndef TILE_H
#define TILE_H

#include <QRectF>
#include <QColor>

// different map tile types
enum class TileType
{
    Stone,
    Wall,
    Swamp,
    Fire,
    Finish
};

class Tile
{
public:
    Tile();
    Tile(TileType type, const QRectF &rect);

    TileType getType() const;
    QRectF getRect() const;

    bool isSolid() const;
    float speedMultiplier() const;
    int damagePerSecond() const;
    QColor color() const;

private:
    TileType type;
    QRectF rect;
};

#endif // TILE_H
