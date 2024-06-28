#include "SpeedSwitcher.h"
#include "Block.h"
#include <QPainter>

SpeedSwitcher::SpeedSwitcher(QPoint pos) {
    ssPos_ = pos;
    occupied = false;
    changeDir_(right);
}

QRectF SpeedSwitcher::boundingRect() const
{
    return QRectF(ssPos_.x(),ssPos_.y(),BLOCK_WIDTH,BLOCK_HEIGHT);
}

void SpeedSwitcher::paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget)
{
    painter->drawPixmap(boundingRect(),ssPix,ssPix.rect());
}

void SpeedSwitcher::changeDir_(int dir)
{
    ssDir_ = (enum direction)dir;

    switch (ssDir_) {
    case right:
        ssPix.load(":/img/resource/ssRight.png");
        break;
    case down:
        ssPix.load(":/img/resource/ssDown.png");
        break;
    case left:
        ssPix.load(":/img/resource/ssLeft.png");
        break;
    case up:
        ssPix.load(":/img/resource/ssUp.png");
        break;
    }

    update();
}
