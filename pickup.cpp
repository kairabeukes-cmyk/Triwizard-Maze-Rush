#include "pickup.h"
#include "spritemanager.h"

#include <QColor>
#include <QFont>

Pickup::Pickup()
    : GameObject(QPointF(0, 0), QSizeF(36, 36)),
      type(PickupType::HealthPotion)
{
}

Pickup::Pickup(PickupType type, const QPointF &position)
    : GameObject(position, QSizeF(38, 38)),
      type(type)
{
}

void Pickup::update(float deltaTime)
{
    Q_UNUSED(deltaTime);
}

void Pickup::draw(QPainter &painter, const QPointF &cameraPosition)
{
    if (!active)
        return;

    QRectF screenRect(position.x() - cameraPosition.x(),
                      position.y() - cameraPosition.y(),
                      size.width(),
                      size.height());

    QString spriteName;

    if (type == PickupType::SpeedBoost)
        spriteName = "speedboost";
    else if (type == PickupType::Shield)
        spriteName = "shield";
    else
        spriteName = "potion";

    if (SpriteManager::drawSprite(painter, spriteName, screenRect))
        return;

    QColor fillColor;
    QString label;

    if (type == PickupType::SpeedBoost)
    {
        fillColor = QColor(90, 210, 255);
        label = "B";
    }
    else if (type == PickupType::Shield)
    {
        fillColor = QColor(110, 130, 255);
        label = "S";
    }
    else
    {
        fillColor = QColor(230, 70, 90);
        label = "+";
    }

    painter.setBrush(fillColor);
    painter.setPen(QColor(245, 245, 255));
    painter.drawEllipse(screenRect);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 15, QFont::Bold));
    painter.drawText(screenRect, Qt::AlignCenter, label);

    painter.setFont(QFont("Arial", 8, QFont::Bold));
    painter.drawText(QRectF(screenRect.left() - 25,
                            screenRect.top() - 17,
                            screenRect.width() + 50,
                            14),
                     Qt::AlignCenter,
                     getName());
}

void Pickup::applyTo(Player *player)
{
    if (!player || !active)
        return;

    if (type == PickupType::SpeedBoost)
    {
        player->activateSpeedBoost(3.0f);
    }
    else if (type == PickupType::Shield)
    {
        player->activateShield(4.0f);
    }
    else if (type == PickupType::HealthPotion)
    {
        player->heal(30);
    }

    active = false;
}

PickupType Pickup::getType() const
{
    return type;
}

QString Pickup::getName() const
{
    if (type == PickupType::SpeedBoost)
        return "Boost";

    if (type == PickupType::Shield)
        return "Shield";

    return "Potion";
}
