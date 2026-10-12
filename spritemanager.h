#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QHash>

// loads and draws sprites from the resource file
class SpriteManager
{
public:
    static bool drawSprite(QPainter &painter,
                           const QString &spriteName,
                           const QRectF &targetRect);

    static bool hasSprite(const QString &spriteName);

private:
    static QPixmap getSprite(const QString &spriteName);

    static QHash<QString, QPixmap> spriteCache;
};

#endif // SPRITEMANAGER_H
