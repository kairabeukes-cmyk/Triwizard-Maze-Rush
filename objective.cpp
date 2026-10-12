#include "objective.h"
#include "spritemanager.h"

#include <QColor>
#include <QFont>

Objective::Objective()
    : GameObject(QPointF(0, 0), QSizeF(40, 40)),
      type(ObjectiveType::Key),
      completed(false),
      health(1)
{
}

Objective::Objective(ObjectiveType type, const QPointF &position)
    : GameObject(position, QSizeF(44, 44)),
      type(type),
      completed(false),
      health(1)
{
    if (type == ObjectiveType::Key)
    {
        size = QSizeF(32, 32);
        health = 1;
    }
    else if (type == ObjectiveType::Crystal)
    {
        size = QSizeF(52, 52);
        health = 3;
    }
    else if (type == ObjectiveType::FinishCup)
    {
        size = QSizeF(52, 52);
        health = 1;
    }
}

void Objective::update(float deltaTime)
{
    Q_UNUSED(deltaTime);
}

void Objective::draw(QPainter &painter, const QPointF &cameraPosition)
{
    if (!active)
        return;

    QRectF screenRect(position.x() - cameraPosition.x(),
                      position.y() - cameraPosition.y(),
                      size.width(),
                      size.height());

    if (type == ObjectiveType::Key)
    {
        if (SpriteManager::drawSprite(painter, "key", screenRect))
            return;

        // fallback key drawing
        painter.setBrush(QColor(240, 210, 70));
        painter.setPen(QColor(255, 245, 180));
        painter.drawEllipse(screenRect);

        painter.setPen(QColor(60, 45, 10));
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(screenRect, Qt::AlignCenter, "K");
    }
    else if (type == ObjectiveType::Crystal)
    {
        QString spriteName = completed ? "dragon_egg_broken" : "dragon_egg";

        if (!SpriteManager::drawSprite(painter, spriteName, screenRect))
        {
            // fallback egg drawing
            QColor crystalColor(120, 220, 255);

            if (completed)
                crystalColor = QColor(80, 80, 90);

            painter.setBrush(crystalColor);
            painter.setPen(QColor(230, 250, 255));
            painter.drawPolygon(QPolygonF()
                                << QPointF(screenRect.center().x(), screenRect.top())
                                << QPointF(screenRect.right(), screenRect.center().y())
                                << QPointF(screenRect.center().x(), screenRect.bottom())
                                << QPointF(screenRect.left(), screenRect.center().y()));
        }

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 9, QFont::Bold));

        if (!completed)
        {
            painter.drawText(QRectF(screenRect.left() - 20,
                                    screenRect.bottom() + 2,
                                    screenRect.width() + 40,
                                    16),
                             Qt::AlignCenter,
                             QString("Egg: %1").arg(health));
        }
        else
        {
            painter.drawText(QRectF(screenRect.left() - 25,
                                    screenRect.bottom() + 2,
                                    screenRect.width() + 50,
                                    16),
                             Qt::AlignCenter,
                             "Complete");
        }
    }
    else if (type == ObjectiveType::FinishCup)
    {
        if (SpriteManager::drawSprite(painter, "cup", screenRect))
            return;

        // fallback cup drawing
        painter.setBrush(QColor(230, 190, 50));
        painter.setPen(QColor(255, 245, 180));
        painter.drawRoundedRect(screenRect, 8, 8);

        painter.setPen(QColor(70, 50, 10));
        painter.setFont(QFont("Arial", 18, QFont::Bold));
        painter.drawText(screenRect, Qt::AlignCenter, "Cup");
    }
}

void Objective::activate(Player *player)
{
    if (!player || completed || !active)
        return;

    if (type == ObjectiveType::Key)
    {
        player->collectKey();
        completed = true;
        active = false;
    }
    else if (type == ObjectiveType::FinishCup)
    {
        player->finish();
        completed = true;
    }
}

void Objective::damage(int amount)
{
    if (type != ObjectiveType::Crystal)
        return;

    if (completed)
        return;

    health -= amount;

    if (health <= 0)
    {
        health = 0;
        completed = true;
    }
}

void Objective::forceComplete()
{
    completed = true;

    if (type == ObjectiveType::Key)
        active = false;

    if (type == ObjectiveType::Crystal)
        health = 0;
}

ObjectiveType Objective::getType() const
{
    return type;
}

bool Objective::isCompleted() const
{
    return completed;
}

int Objective::getHealth() const
{
    return health;
}
