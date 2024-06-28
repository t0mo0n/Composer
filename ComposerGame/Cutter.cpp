#include "Cutter.h"
#include <QPainter>
#include <QTimer>
#include "Block.h"
#include "Note.h"
#include "PlayWindow.h"


Cutter::Cutter(int level, QPoint pos_)
{
    occupied = false;
    noteToCutType = 0;
    cutterLv_ = (enum levels) level;
    cutterPos_1 = pos_;
    changeDir_(right);

    generateTimer = new QTimer();
    connect(generateTimer,&QTimer::timeout,this,&Cutter::generateNote_slot);
}

Cutter::~Cutter()
{
    occupied = false;
    delete generateTimer;
    generateTimer = nullptr;
}

QRectF Cutter::boundingRect() const
{
    switch (cutterDir_) {
    case right:
        return QRectF(cutterPos_2.x(),cutterPos_2.y(),BLOCK_WIDTH,2*BLOCK_HEIGHT);
    case down:
        return QRectF(cutterPos_1.x(),cutterPos_1.y(),2*BLOCK_WIDTH,BLOCK_HEIGHT);
    case left:
        return QRectF(cutterPos_1.x(),cutterPos_1.y(),BLOCK_WIDTH,2*BLOCK_HEIGHT);
    default:
        return QRectF(cutterPos_2.x(),cutterPos_2.y(),2*BLOCK_WIDTH,BLOCK_HEIGHT);
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
        cutterPos_2 = QPoint(cutterPos_1.x()-BLOCK_WIDTH,cutterPos_1.y());
        cutterPix.load(":/img/resource/CutterUp.png");
        break;
    case down:
        cutterPos_2 = QPoint(cutterPos_1.x()+BLOCK_WIDTH,cutterPos_1.y());
        cutterPix.load(":/img/resource/CutterDown.png");
        break;
    case left:
        cutterPos_2 = QPoint(cutterPos_1.x(),cutterPos_1.y()+BLOCK_HEIGHT);
        cutterPix.load(":/img/resource/CutterLeft.png");
        break;
    case right:
        cutterPos_2 = QPoint(cutterPos_1.x(),cutterPos_1.y()-BLOCK_HEIGHT);
        cutterPix.load(":/img/resource/CutterRight.png");
        break;
    }
    update();
}

void Cutter::generateNote(int noteCuttedType,int v)
{
    if (occupied){
        return;
    }
    noteToCutType = noteCuttedType;
    noteInitSpeed = v;
    if (cutterLv_ == 2){
        generateTimer->start(1500);
    } else{
        generateTimer->start(3000);
    }
}

void Cutter::generateNote_slot()
{
    switch (noteToCutType) {
    case 0:{
        occupied = 1;
        QSharedPointer<Note> newNote_Empty = PlayWindow::notePool->acquire();
        newNote_Empty->initNote(noteInitSpeed,0,cutterPos_1);
        PlayWindow::setNoteToScene(newNote_Empty);
        break;
    }
    case 1:{
        occupied = 2;
        QSharedPointer<Note> newNote_1 = PlayWindow::notePool->acquire();
        QSharedPointer<Note> newNote_2 = PlayWindow::notePool->acquire();
        newNote_1->initNote(noteInitSpeed,2,cutterPos_1);
        newNote_2->initNote(noteInitSpeed,3,cutterPos_2);
        PlayWindow::setNoteToScene(newNote_1);
        PlayWindow::setNoteToScene(newNote_2);
        break;
    }
    case 2:{
        occupied = 2;
        QSharedPointer<Note> newNote_1 = PlayWindow::notePool->acquire();
        QSharedPointer<Note> newNote_2 = PlayWindow::notePool->acquire();
        newNote_1->initNote(noteInitSpeed,4,cutterPos_1);
        newNote_2->initNote(noteInitSpeed,5,cutterPos_2);
        PlayWindow::setNoteToScene(newNote_1);
        PlayWindow::setNoteToScene(newNote_2);
        break;
    }
    case 3:{
        occupied = 2;
        QSharedPointer<Note> newNote_1 = PlayWindow::notePool->acquire();
        QSharedPointer<Note> newNote_2 = PlayWindow::notePool->acquire();
        newNote_1->initNote(noteInitSpeed,6,cutterPos_1);
        newNote_2->initNote(noteInitSpeed,7,cutterPos_2);
        PlayWindow::setNoteToScene(newNote_1);
        PlayWindow::setNoteToScene(newNote_2);
        break;
    }
    case 4:{
        occupied = 1;
        QSharedPointer<Note> newNote = PlayWindow::notePool->acquire();
        newNote->initNote(noteInitSpeed,4,cutterPos_1);
        PlayWindow::setNoteToScene(newNote);
        break;
    }
    case 5:{
        occupied = 2;
        QSharedPointer<Note> newNote_1 = PlayWindow::notePool->acquire();
        QSharedPointer<Note> newNote_2 = PlayWindow::notePool->acquire();
        newNote_1->initNote(noteInitSpeed,5,cutterPos_1);
        newNote_2->initNote(noteInitSpeed,5,cutterPos_2);
        PlayWindow::setNoteToScene(newNote_1);
        PlayWindow::setNoteToScene(newNote_2);
        break;
    }
    case 6:{
        occupied = 1;
        QSharedPointer<Note> newNote = PlayWindow::notePool->acquire();
        newNote->initNote(noteInitSpeed,6,cutterPos_1);
        PlayWindow::setNoteToScene(newNote);
        break;
    }
    case 7:{
        occupied = 1;
        QSharedPointer<Note> newNote = PlayWindow::notePool->acquire();
        newNote->initNote(noteInitSpeed,7,cutterPos_1);
        PlayWindow::setNoteToScene(newNote);
        break;
    }
    default:
        break;
    }
    generateTimer->stop();
}
