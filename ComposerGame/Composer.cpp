#include "Composer.h"
#include <QPainter>
#include "Block.h"


Composer::Composer(int level,QPoint pos_)
{
    composerLv_ = (enum levels)level;

    composerPos_ = pos_;

    changeDir_(right);
}

QRectF Composer::boundingRect() const
{
    return QRectF(composerPos_.x(),composerPos_.y(),BLOCK_WIDTH,BLOCK_HEIGHT);
}

void Composer::paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget)
{
    painter->drawPixmap(boundingRect(),composerPix,composerPix.rect());
}

void Composer::changeDir_(int dir)
{
    composerDir_ = (enum direction)dir;

    switch (composerDir_) {
    case right:
        composerPix.load(":/img/resource/ComposerRight.png");
        break;
    case down:
        composerPix.load(":/img/resource/ComposerDown.png");
        break;
    case left:
        composerPix.load(":/img/resource/ComposerLeft.png");
        break;
    case up:
        composerPix.load(":/img/resource/ComposerUp.png");
        break;
    }

    update();
}


