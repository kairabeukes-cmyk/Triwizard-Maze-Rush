#include "gameobject.h"

GameObject::GameObject()
    : position(0, 0),
      size(32, 32),
      velocity(0, 0),
      active(true)
{
}

GameObject::GameObject(const QPointF &position, const QSizeF &size)
    : position(position),
      size(size),
      velocity(0, 0),
      active(true)
{
}

GameObject::~GameObject()
{
}

void GameObject::update(float deltaTime)
{
    Q_UNUSED(deltaTime);
}

void GameObject::draw(QPainter &painter, const QPointF &cameraPosition)
{
    QRectF screenRect(position.x() - cameraPosition.x(),
                      position.y() - cameraPosition.y(),
                      size.width(),
                      size.height());

    // fallback colour for missing draw code
    painter.fillRect(screenRect, Qt::magenta);
}

QRectF GameObject::boundingBox() const
{
    return QRectF(position.x(),
                  position.y(),
                  size.width(),
                  size.height());
}

QPointF GameObject::getPosition() const
{
    return position;
}

void GameObject::setPosition(const QPointF &newPosition)
{
    position = newPosition;
}

QSizeF GameObject::getSize() const
{
    return size;
}

void GameObject::setSize(const QSizeF &newSize)
{
    size = newSize;
}

QPointF GameObject::getVelocity() const
{
    return velocity;
}

void GameObject::setVelocity(const QPointF &newVelocity)
{
    velocity = newVelocity;
}

bool GameObject::isActive() const
{
    return active;
}

void GameObject::setActive(bool value)
{
    active = value;
}
