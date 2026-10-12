#include "tilemap.h"
#include "spritemanager.h"

TileMap::TileMap()
    : tileSize(64)
{
    loadDefaultMap();
}

void TileMap::loadDefaultMap()
{
    // map legend: # wall, . floor, s swamp, f fire, e finish
    QStringList rows;

    rows << "#######...######........................";
    rows << "#.....#.###....#.############..#######..";
    rows << "#.....##.......##.#.........#..#.....#..";
    rows << "#........##.......#.........#..#.....#..";
    rows << "#.....##.########.#.........####.....##.";
    rows << "######.#.#...#..#.#...................#.";
    rows << ".......#.#...####.#.........###.......#.";
    rows << ".#######.#...#....####.######.######.##.";
    rows << ".#.......##..#.##....#.#...........#.#..";
    rows << ".#.#######.S.S..###..#.#.#######.###.###";
    rows << ".#.#......#...S.SS####.###.....###.....#";
    rows << ".#.#...####SS..S...SS.G.#..............#";
    rows << ".#.#####....SS..SS.###.###.....###.#####";
    rows << ".#.#...###SSS.SS.S##.#.#.#.....#.#.#....";
    rows << ".#.......#.S.S....#..#.#.#######.#.###..";
    rows << ".###.....#..SS.####.##.#........#...F##.";
    rows << "...#####.####..#....#..##########FF...#.";
    rows << ".......#....######..#.....#.........FF#.";
    rows << "...#####.##....FF#.##.######..###FF...#.";
    rows << "...#......#...FF.###..#.........########";
    rows << "...#.######FF.........#...........#...##";
    rows << "...#.#...##.....F.#####...........#....#";
    rows << "..##.###.##.FFF.FF#.#.............#...E#";
    rows << "..#.....###.......###..................#";
    rows << "..#.......#.##FF.F..#.............#...E#";
    rows << "..#######.##.#...F###.............#....#";
    rows << "........#.#..#.FF.#.#...........###...##";
    rows << "........###..######.#############.######";

    const int targetCols = 40;

    // keep all rows the same length
    for (int i = 0; i < rows.size(); ++i)
    {
        if (rows[i].size() < targetCols)
        {
            while (rows[i].size() < targetCols)
                rows[i].append("#");
        }
        else if (rows[i].size() > targetCols)
        {
            rows[i] = rows[i].left(targetCols);
        }
    }

    map.clear();

    for (int r = 0; r < rows.size(); ++r)
    {
        QVector<TileType> row;

        for (int c = 0; c < rows[r].size(); ++c)
            row.append(charToTileType(rows[r][c]));

        map.append(row);
    }
}

void TileMap::draw(QPainter &painter, const QPointF &cameraPosition) const
{
    for (int r = 0; r < map.size(); ++r)
    {
        for (int c = 0; c < map[r].size(); ++c)
        {
            QRectF worldRect(c * tileSize,
                             r * tileSize,
                             tileSize,
                             tileSize);

            QRectF screenRect(worldRect.x() - cameraPosition.x(),
                              worldRect.y() - cameraPosition.y(),
                              worldRect.width(),
                              worldRect.height());

            if (screenRect.right() < 0 ||
                screenRect.left() > painter.device()->width() ||
                screenRect.bottom() < 0 ||
                screenRect.top() > painter.device()->height())
            {
                continue;
            }

            Tile tile(map[r][c], worldRect);

            QString spriteName = spriteNameForTile(r, c);

            bool spriteDrawn = SpriteManager::drawSprite(painter,
                                                          spriteName,
                                                          screenRect);

            if (!spriteDrawn)
                painter.fillRect(screenRect, tile.color());

            painter.setPen(QColor(20, 20, 25, 80));
            painter.drawRect(screenRect);
        }
    }
}

TileType TileMap::getTileTypeAtPixel(const QPointF &pixelPosition) const
{
    int col = static_cast<int>(pixelPosition.x()) / tileSize;
    int row = static_cast<int>(pixelPosition.y()) / tileSize;

    if (row < 0 || row >= map.size())
        return TileType::Wall;

    if (col < 0 || col >= map[row].size())
        return TileType::Wall;

    return map[row][col];
}

bool TileMap::isSolidAtPixel(const QPointF &pixelPosition) const
{
    TileType type = getTileTypeAtPixel(pixelPosition);
    Tile tile(type, QRectF());

    return tile.isSolid();
}

bool TileMap::rectCollidesWithSolidTile(const QRectF &rect) const
{
    QPointF topLeft = rect.topLeft();
    QPointF topRight = rect.topRight();
    QPointF bottomLeft = rect.bottomLeft();
    QPointF bottomRight = rect.bottomRight();

    if (isSolidAtPixel(topLeft))
        return true;

    if (isSolidAtPixel(topRight))
        return true;

    if (isSolidAtPixel(bottomLeft))
        return true;

    if (isSolidAtPixel(bottomRight))
        return true;

    return false;
}

int TileMap::getTileSize() const
{
    return tileSize;
}

int TileMap::getRows() const
{
    return map.size();
}

int TileMap::getCols() const
{
    if (map.isEmpty())
        return 0;

    return map[0].size();
}

int TileMap::getWidthPixels() const
{
    return getCols() * tileSize;
}

int TileMap::getHeightPixels() const
{
    return getRows() * tileSize;
}

TileType TileMap::charToTileType(QChar ch) const
{
    if (ch == '#')
        return TileType::Wall;

    if (ch == 'S')
        return TileType::Swamp;

    if (ch == 'F')
        return TileType::Fire;

    if (ch == 'E')
        return TileType::Finish;

    return TileType::Stone;
}

QString TileMap::spriteNameForTile(int row, int col) const
{
    TileType type = map[row][col];

    if (type == TileType::Wall)
        return "wall";

    if (type == TileType::Swamp)
        return "swamp";

    if (type == TileType::Fire)
        return "fire";

    if (type == TileType::Finish)
        return "finish";

    return "stone";
}
