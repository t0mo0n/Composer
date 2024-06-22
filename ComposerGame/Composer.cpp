#include "Composer.h"
#include <QPainter>
#include "Block.h"


Composer::Composer(int level,QPointF pos_)
{
    composerLv_ = (enum levels)level;

    composerPos_ = pos_;

    composerDir_ = up;
}

QRectF Composer::boundingRect() const
{
    return QRectF(composerPos_.x(),composerPos_.y(),BLOCK_WIDTH,BLOCK_HEIGHT);
}

void Composer::paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget)
{
    QPixmap composerPix;
    composerPix.load(":/img/resource/Compose.png");
    painter->drawPixmap(boundingRect(),composerPix,composerPix.rect());
}


