#include "Center.h"
#include "Block.h"
#include <QPainter>

Center::Center(int level, QPoint centerposition__) {
    centerLv_ = (enum level)level;

    centerPos_ = centerposition__;

    if (centerLv_ == lv_2){
        cWidth = 7;
        cHeight = 7;
    } else{
        cWidth = 5;
        cHeight = 5;
    }
}

QRectF Center::boundingRect() const
{
    if (centerLv_ == lv_2){
        return QRectF(centerPos_.x(),centerPos_.y(),7*BLOCK_WIDTH,7*BLOCK_HEIGHT);
    } else{
        return QRectF(centerPos_.x(),centerPos_.y(),5*BLOCK_WIDTH,5*BLOCK_HEIGHT);
    }
}

void Center::paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget)
{
    QPixmap centerPix;
    centerPix.load(":/img/resource/Center_1.png");
    painter->drawPixmap(boundingRect(),centerPix,centerPix.rect());
}

void Center::mousePressEvent(QGraphicsSceneMouseEvent *)
{

}

void Center::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{

}
