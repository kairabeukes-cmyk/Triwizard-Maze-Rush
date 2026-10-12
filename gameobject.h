#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QPainter>

// base class for objects that appear in the game world
class GameObject
{
public:
    GameObject();
    GameObject(const QPointF &position, const QSizeF &size);
    virtual ~GameObject();

    virtual void update(float deltaTime);
    virtual void draw(QPainter &painter, const QPointF &cameraPosition);

    QRectF boundingBox() const; // used for collision

    QPointF getPosition() const;
    void setPosition(const QPointF &newPosition);

    QSizeF getSize() const;
    void setSize(const QSizeF &newSize);

    QPointF getVelocity() const;
    void setVelocity(const QPointF &newVelocity);

    bool isActive() const;
    void setActive(bool value);

protected:
    QPointF position; // world position
    QSizeF size;      // object size
    QPointF velocity; // movement per second
    bool active;      // controls if object is used
};

#endif // GAMEOBJECT_H
