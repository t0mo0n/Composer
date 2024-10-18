#include "Belt.h"
#include "Block.h"
#include <QPainter>

void Belt::initBelt(int initDir, QPoint pos) {
    beltPos_ = pos;
    occupied = false;

    changeDir_(initDir); // 初始方向仅四个(1~4),也就是四个最基本的方向
}

QRectF Belt::boundingRect() const
{
    return QRectF(beltPos_.x(),beltPos_.y(),BLOCK_WIDTH,BLOCK_HEIGHT);
}

void Belt::paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget)
{
    painter->setOpacity(0.7);
    painter->drawPixmap(boundingRect(),beltPix,beltPix.rect());
    painter->setOpacity(1.0);
}

void Belt::deleteBelt()
{
    occupied = false;
    changeDir_(LeftToRight);
}

void Belt::changeDir_(int dir)
{
    beltDir_ = (enum direction)dir;

    switch (beltDir_) {
    case LeftToRight:
        beltPix.load(":/img/resource/LR.png");
        break;
    case UpToDown:
        beltPix.load(":/img/resource/UD.png");
        break;
    case RightToLeft:
        beltPix.load(":/img/resource/RL.png");
        break;
    case DownToUp:
        beltPix.load(":/img/resource/DU.png");
        break;
    case DownToRight:
        beltPix.load(":/img/resource/DR.png");
        break;
    case DownToLeft:
        beltPix.load(":/img/resource/DL.png");
        break;
    case LeftToDown:
        beltPix.load(":/img/resource/LD.png");
        break;
    case LeftToUp:
        beltPix.load(":/img/resource/LU.png");
        break;
    case UpToLeft:
        beltPix.load(":/img/resource/UL.png");
        break;
    case UpToRight:
        beltPix.load(":/img/resource/UR.png");
        break;
    case RightToUp:
        beltPix.load(":/img/resource/RU.png");
        break;
    case RightToDown:
        beltPix.load(":/img/resource/RD.png");
        break;
    default:
        // LeftToRight
        beltPix.load(":/img/resource/LR.png");
        break;
    }

    update();
}
