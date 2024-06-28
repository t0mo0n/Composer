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
    painter->drawPixmap(boundingRect(),beltPix,beltPix.rect());
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
        beltPix.load(":/img/resource/Belt_LeftToRight.png");
        break;
    case UpToDown:
        beltPix.load(":/img/resource/Belt_UpToDown.png");
        break;
    case RightToLeft:
        beltPix.load(":/img/resource/Belt_RightToLeft.png");
        break;
    case DownToUp:
        beltPix.load(":/img/resource/Belt_DownToUp.png");
        break;
    case DownToRight:
        beltPix.load(":/img/resource/Belt_DownToRight.png");
        break;
    case DownToLeft:
        beltPix.load(":/img/resource/Belt_DownToLeft.png");    
        break;
    case LeftToDown:
        beltPix.load(":/img/resource/Belt_LeftToDown.png");    
        break;
    case LeftToUp:
        beltPix.load(":/img/resource/Belt_LeftToUp.png");
        break;
    case UpToLeft:
        beltPix.load(":/img/resource/Belt_UpToLeft.png");
        break;
    case UpToRight:
        beltPix.load(":/img/resource/Belt_UpToRight.png");      
        break;
    case RightToUp:
        beltPix.load(":/img/resource/Belt_RightToUp.png");
        break;
    case RightToDown:
        beltPix.load(":/img/resource/Belt_RightToDown.png");
        break;
    default:
        // LeftToRight
        beltPix.load(":/img/resource/Belt_LeftToRight.png");
        break;
    }

    update();
}
