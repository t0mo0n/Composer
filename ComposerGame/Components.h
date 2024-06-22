#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <QGraphicsObject>

class Components : public QGraphicsObject
{
    Q_OBJECT
public:
    Components();
    void mousePressEvent(QGraphicsSceneMouseEvent *) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *) override;
};

#endif // COMPONENTS_H
