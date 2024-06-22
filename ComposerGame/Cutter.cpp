#include "Cutter.h"
#include <QPainter>
#include "Block.h"


Cutter::Cutter(int level, QPointF pos_)
{
    cutterLv_ = (enum levels) level;

    cutterPos_ = pos_;

    cutterDir_ = up;
}

QRectF Cutter::boundingRect() const
{
    return QRectF(cutterPos_.x(),cutterPos_.y(),2*BLOCK_WIDTH,BLOCK_HEIGHT);
}

void Cutter::paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget)
{
    QPixmap cutterPix;
    cutterPix.load(":/img/resource/Cutter.png");
    painter->drawPixmap(boundingRect(),cutterPix,cutterPix.rect());
}
