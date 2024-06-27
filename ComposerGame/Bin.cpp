#include "Bin.h"
#include "Block.h"
#include <QPainter>


Bin::Bin(QPoint pos)
{
    binPos_ = pos;
}

QRectF Bin::boundingRect() const
{
    return QRectF(binPos_.x(),binPos_.y(),BLOCK_WIDTH,BLOCK_HEIGHT);
}

void Bin::paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget)
{
    QPixmap binPix;
    binPix.load(":/img/resource/Bin.png");
    painter->drawPixmap(boundingRect(),binPix,binPix.rect());
}

