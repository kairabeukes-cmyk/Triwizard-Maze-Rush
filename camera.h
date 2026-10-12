#ifndef CAMERA_H
#define CAMERA_H

#include <QPointF>
#include <QSizeF>
#include <QRectF>

class Camera
{
public:
    Camera();

    void setViewportSize(const QSizeF &size); // stores window size
    void setWorldSize(const QSizeF &size);    // stores map size

    void follow(const QPointF &targetPosition, const QSizeF &targetSize); // follows player

    QPointF getPosition() const;
    QRectF getViewRect() const;

private:
    QPointF position;       // top-left camera position
    QSizeF viewportSize;    // visible screen size
    QSizeF worldSize;       // full map size

    void clampToWorld();    // stops camera leaving the map
};

#endif // CAMERA_H
