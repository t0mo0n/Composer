#include "Cutter.h"
#include <QPainter>
#include "Block.h"


Cutter::Cutter(int level, QPoint pos_)
{
    cutterLv_ = (enum levels) level;

    cutterPos_1 = pos_;

    changeDir_(right);
}

QRectF Cutter::boundingRect() const
{
    switch (cutterDir_) {
    case right:
        return QRectF(cutterPos_1.x(),cutterPos_1.y(),BLOCK_WIDTH,2*BLOCK_HEIGHT);
    case down:
        return QRectF(cutterPos_2.x(),cutterPos_2.y(),2*BLOCK_WIDTH,BLOCK_HEIGHT);
    case left:
        return QRectF(cutterPos_2.x(),cutterPos_2.y(),BLOCK_WIDTH,2*BLOCK_HEIGHT);
    default:
        return QRectF(cutterPos_1.x(),cutterPos_1.y(),2*BLOCK_WIDTH,BLOCK_HEIGHT);
    }
}

void Cutter::paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget)
{
    painter->drawPixmap(boundingRect(),cutterPix,cutterPix.rect());
}

void Cutter::changeDir_(int dir)
{
    prepareGeometryChange();
    cutterDir_ = (enum direction)dir;
    switch (cutterDir_) {
    case up:
        cutterPos_2 = QPoint(cutterPos_1.x()+BLOCK_WIDTH,cutterPos_1.y());
        cutterPix.load(":/img/resource/CutterUp.png");
        break;
    case down:
        cutterPos_2 = QPoint(cutterPos_1.x()-BLOCK_WIDTH,cutterPos_1.y());
        cutterPix.load(":/img/resource/CutterDown.png");
        break;
    case left:
        cutterPos_2 = QPoint(cutterPos_1.x(),cutterPos_1.y()-BLOCK_HEIGHT);
        cutterPix.load(":/img/resource/CutterLeft.png");
        break;
    case right:
        cutterPos_2 = QPoint(cutterPos_1.x(),cutterPos_1.y()+BLOCK_HEIGHT);
        cutterPix.load(":/img/resource/CutterRight.png");
        break;
    }
    update();
}
