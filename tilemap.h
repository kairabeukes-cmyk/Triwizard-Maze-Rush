#ifndef TILEMAP_H
#define TILEMAP_H

#include <QVector>
#include <QStringList>
#include <QPainter>
#include <QPointF>
#include <QRectF>

#include "tile.h"

// stores the maze layout and checks tile collisions
class TileMap
{
public:
    TileMap();

    void loadDefaultMap();
    void draw(QPainter &painter, const QPointF &cameraPosition) const;

    TileType getTileTypeAtPixel(const QPointF &pixelPosition) const;
    bool isSolidAtPixel(const QPointF &pixelPosition) const;
    bool rectCollidesWithSolidTile(const QRectF &rect) const;

    int getTileSize() const;
    int getRows() const;
    int getCols() const;
    int getWidthPixels() const;
    int getHeightPixels() const;

private:
    QVector<QVector<TileType> > map;
    int tileSize;

    TileType charToTileType(QChar ch) const;       // converts maze characters
    QString spriteNameForTile(int row, int col) const;
};

#endif // TILEMAP_H
