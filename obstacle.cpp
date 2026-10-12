#include "obstacle.h"
#include "spritemanager.h"

#include <QColor>
#include <QFont>

Obstacle::Obstacle()
    : GameObject(QPointF(0, 0), QSizeF(64, 64)),
      type(ObstacleType::LockedGate),
      open(false)
{
}

Obstacle::Obstacle(ObstacleType type, const QPointF &position, const QSizeF &size)
    : GameObject(position, size),
      type(type),
      open(false)
{
}

void Obstacle::update(float deltaTime)
{
    Q_UNUSED(deltaTime);
}

void Obstacle::draw(QPainter &painter, const QPointF &cameraPosition)
{
    if (!active)
        return;

    QRectF screenRect(position.x() - cameraPosition.x(),
                      position.y() - cameraPosition.y(),
                      size.width(),
                      size.height());

    if (type == ObstacleType::LockedGate)
    {
        QString spriteName = open ? "gate_open" : "gate_closed";

        if (SpriteManager::drawSprite(painter, spriteName, screenRect))
            return;

        if (open)
        {
            painter.setBrush(QColor(80, 140, 90, 120));
            painter.setPen(QColor(140, 220, 150));
            painter.drawRect(screenRect);

            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(screenRect, Qt::AlignCenter, "OPEN");
        }
        else
        {
            painter.setBrush(QColor(95, 55, 25));
            painter.setPen(QColor(220, 170, 90));
            painter.drawRect(screenRect);

            painter.setPen(QColor(255, 230, 120));
            painter.setFont(QFont("Arial", 11, QFont::Bold));
            painter.drawText(screenRect, Qt::AlignCenter, "GATE");
        }
    }
}

void Obstacle::interact(Player *player)
{
    if (!player)
        return;

    if (type == ObstacleType::LockedGate)
    {
        if (player->ownsKey())
            open = true;
    }
}

void Obstacle::forceOpen()
{
    open = true;
}

ObstacleType Obstacle::getType() const
{
    return type;
}

bool Obstacle::isOpen() const
{
    return open;
}

bool Obstacle::isSolid() const
{
    if (type == ObstacleType::LockedGate)
        return !open;

    return false;
}
