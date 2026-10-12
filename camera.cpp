#include "camera.h"

Camera::Camera()
    : position(0, 0),
      viewportSize(1000, 700),
      worldSize(1000, 700)
{
}

void Camera::setViewportSize(const QSizeF &size)
{
    viewportSize = size;
    clampToWorld();
}

void Camera::setWorldSize(const QSizeF &size)
{
    worldSize = size;
    clampToWorld();
}

void Camera::follow(const QPointF &targetPosition, const QSizeF &targetSize)
{
    // center the camera on the target
    QPointF targetCenter(targetPosition.x() + targetSize.width() / 2.0,
                         targetPosition.y() + targetSize.height() / 2.0);

    position.setX(targetCenter.x() - viewportSize.width() / 2.0);
    position.setY(targetCenter.y() - viewportSize.height() / 2.0);

    clampToWorld();
}

QPointF Camera::getPosition() const
{
    return position;
}

QRectF Camera::getViewRect() const
{
    return QRectF(position, viewportSize);
}

void Camera::clampToWorld()
{
    // keep the camera inside the map
    if (position.x() < 0)
        position.setX(0);

    if (position.y() < 0)
        position.setY(0);

    double maxX = worldSize.width() - viewportSize.width();
    double maxY = worldSize.height() - viewportSize.height();

    if (maxX < 0)
        maxX = 0;

    if (maxY < 0)
        maxY = 0;

    if (position.x() > maxX)
        position.setX(maxX);

    if (position.y() > maxY)
        position.setY(maxY);
}
