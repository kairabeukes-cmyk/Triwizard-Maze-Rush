#include "spritemanager.h"

QHash<QString, QPixmap> SpriteManager::spriteCache;

bool SpriteManager::drawSprite(QPainter &painter,
                               const QString &spriteName,
                               const QRectF &targetRect)
{
    QPixmap pixmap = getSprite(spriteName);

    if (pixmap.isNull())
        return false;

    painter.drawPixmap(targetRect.toRect(), pixmap);
    return true;
}

bool SpriteManager::hasSprite(const QString &spriteName)
{
    return !getSprite(spriteName).isNull();
}

QPixmap SpriteManager::getSprite(const QString &spriteName)
{
    if (spriteCache.contains(spriteName))
        return spriteCache.value(spriteName);

    QString path = QString(":/sprites/%1.png").arg(spriteName);

    QPixmap pixmap(path);

    spriteCache.insert(spriteName, pixmap);

    return pixmap;
}
