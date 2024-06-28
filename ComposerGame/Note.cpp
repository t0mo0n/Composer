#include "Note.h"
#include <QPainter>
#include "Block.h"
#include "PlayWindow.h"

Note::Note() {
    // TODO：要干啥？
}

QRectF Note::boundingRect() const
{
    return QRectF(0,0,NOTE_WIDTH,NOTE_HEIGHT);
}

void Note::paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget)
{
    QPixmap notePix;
    switch (noteType) {
    case Empty:
        notePix.load(":/img/resource/EmptyNote.png");
        break;
    case Do:
        notePix.load(":/img/resource/Do.png");
        break;
    case Re:
        notePix.load(":/img/resource/Re.png");
        break;
    case Mi:
        notePix.load(":/img/resource/Mi.png");
        break;
    case Fa:
        notePix.load(":/img/resource/Fa.png");
        break;
    case So:
        notePix.load(":/img/resource/So.png");
        break;
    case La:
        notePix.load(":/img/resource/La.png");
        break;
    case Si:
        notePix.load(":/img/resource/Si.png");
        break;
    }
    painter->drawPixmap(boundingRect(),notePix,notePix.rect());
}

void Note::initNote(int s, int type, QPoint _pos)
{
    typo_ = 0;
    stop = false;
    noteSpeed_ = s;
    noteType = (enum types)type;
    boundedPos_ = _pos;
    notePos_ = QPoint(boundedPos_.x()+(BLOCK_WIDTH-NOTE_WIDTH)/2,boundedPos_.y()+(BLOCK_HEIGHT-NOTE_HEIGHT)/2);
    setPos(notePos_);
    // dir是生成方块的出口朝向！
    // 下面这个是对应于移动的逻辑的位置定义；
    // if (_dir == QPoint(1,0)){
    //     notePos_ = QPoint(boundedPos_.x()-NOTE_WIDTH/2, boundedPos_.y()+(BLOCK_HEIGHT-NOTE_HEIGHT)/2);
    // } else if(_dir == QPoint(-1,0)){
    //     notePos_ = QPoint(boundedPos_.x()+BLOCK_WIDTH-NOTE_WIDTH/2, boundedPos_.y()+(BLOCK_HEIGHT-NOTE_HEIGHT)/2);
    // } else if(_dir == QPoint(0,1)){
    //     notePos_ = QPoint(boundedPos_.x()+(BLOCK_WIDTH-NOTE_WIDTH)/2, boundedPos_.y()+BLOCK_HEIGHT-NOTE_HEIGHT/2);
    // } else if(_dir == QPoint(0,-1)){
    //     notePos_ = QPoint(boundedPos_.x()+(BLOCK_WIDTH-NOTE_WIDTH)/2,boundedPos_.y()-NOTE_HEIGHT/2);
    // }
}

void Note::changeSpeed()
{
    if (noteSpeed_ == PlayWindow::initSpeed){
        noteSpeed_ = 1;
    } else{
        noteSpeed_ = PlayWindow::initSpeed;
    }
}

void Note::moveBy(qreal dx, qreal dy)
{
    if (dx > 0 && dy == 0){
        speedDir_ = right;
    } else if(dx < 0 && dy == 0){
        speedDir_ = left;
    } else if(dx == 0 && dy >0){
        speedDir_ = down;
    } else{
        speedDir_ = up;
    }
    this->QGraphicsItem::moveBy(dx,dy);
}

void Note::noteDiscard()
{
    typo_ = 0;
    stop = false;
    noteSpeed_ = 0;
    noteType = (enum types)0;
}

