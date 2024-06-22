#ifndef COMPOSER_H
#define COMPOSER_H

#include "Components.h"

class Composer : public Components
{
public:
    Composer(int level,QPointF pos_);
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    // void generateNote(); // TODO

private:
    QPointF composerPos_;
    enum direction {up=1,down,left,right} composerDir_;
    enum levels { lv0, lv1} composerLv_;
};

#endif // COMPOSER_H
