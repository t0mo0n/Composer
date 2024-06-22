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

// void Block::mousePressEvent(QGraphicsSceneMouseEvent *event)
// {
//     if (contains(event->pos()) && event->button() == Qt::LeftButton){
//         pressed = true;
//         if (PlayWindow::toolType == PlayWindow::belt){
//             //  连续放置传送带
//         } else{
//             ;
//         }
//     }
// }

// void Block::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
// {
//     if (pressed && contains(event->pos()) && event->button() == Qt::LeftButton) {
//         pressed = false;
//         switch (PlayWindow::toolType) {
//         case PlayWindow::belt:
//             ; // 什么都不做结束，放置在按下完成
//             break;
//         case PlayWindow::composer:{
//             Composer *new_composer = new Composer(PlayWindow::composerLevel,blockPos_);
//             PlayWindow::composers_.push_back(new_composer);
//             new_composer->setZValue(1);
//             scene()->addItem(new_composer);
//             occupied = true;
//             break;
//         }
//         case PlayWindow::cutter:{
//             Cutter *new_cutter = new Cutter(PlayWindow::cutterLevel,blockPos_);
//             PlayWindow::cutters_.push_back(new_cutter);
//             new_cutter->setZValue(1);
//             scene()->addItem(new_cutter);
//             occupied = true;
//             break;
//         }
//         case PlayWindow::bin:{

//             break;
//         }
//         case PlayWindow::speedSwitcher:{

//             break;
//         }
//         }
//     } else {
//         pressed = false;
//         if (PlayWindow::toolType == PlayWindow::belt){
//             // 结束放置传送带
//         } else{
//             ; // 什么都不做
//         }
//     }
// }

void Block::setBlockPos_(QPoint pos)
{
    blockPos_ = pos;
}

QPoint &Block::getBlockPos_()
{
    return blockPos_;
}
