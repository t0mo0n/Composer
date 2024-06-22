#ifndef CUTTER_H
#define CUTTER_H

#include "Components.h"

class Cutter : public Components
{
public:
    Cutter(int level,QPointF pos_);
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

private:
    QPointF cutterPos_;
    enum direction {up=1,down,left,right} cutterDir_;
    enum levels { lv0, lv1} cutterLv_;
};

#endif // CUTTER_H
