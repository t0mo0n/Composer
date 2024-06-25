#include "Block.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDebug>

Block::Block(int type) {
    if (type == normal_block){
        bType = normal_block;
    } else if (type == inspire_block){
        bType = inspire_block;
    } else{
        bType = empty_block;
    }

    occupied = false;

    bContent = empty;
}

QRectF Block::boundingRect() const
{
    return QRectF(blockPos_.x(),blockPos_.y(),BLOCK_WIDTH,BLOCK_HEIGHT);
}

void Block::paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget)
{
    QPixmap block_pix;
    switch (bType) {
    case normal_block:
        block_pix.load(":/img/resource/normalBlock.png");
        break;
    case inspire_block:
        block_pix.load(":/img/resource/musicBlock.png");
        break;
    case empty_block:
        block_pix.load(":/img/resource/emptyBlock.png");
        break;
    }
    painter->drawPixmap(boundingRect(),block_pix,block_pix.rect());
}

void Block::setBlockPos_(QPoint pos)
{
    blockPos_ = pos;
}
