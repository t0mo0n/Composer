#include "Composer.h"
#include <QPainter>
#include <QTimer>
#include "Block.h"
#include "Note.h"
#include "PlayWindow.h"


Composer::Composer(int level,int blockT,QPoint pos_)
{
    occupied = false;
    composerLv_ = (enum levels)level;
    cGType_ = (enum cgBts)blockT;
    composerPos_ = pos_;
    changeDir_(right);

    generateTimer = new QTimer();
    connect(generateTimer,&QTimer::timeout,this,&Composer::generateNote);
    if (composerLv_ == 2){
        generateTimer->start(2000);
    } else{
        generateTimer->start(4000);
    }
}

Composer::~Composer()
{
    generateTimer->stop();
    delete generateTimer;
    generateTimer = nullptr;
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

void Composer::generateNote()
{
    // 判断是否已经有音符，如果没有，在自己这里生成，修改occupied值然后初始化即可，不用做别的事
    if (occupied){
        return;
    } else if(cGType_ == normal_block){
        return;
    } else{
        occupied = true;
        QSharedPointer<Note> noteNew_ = PlayWindow::notePool->acquire();
        if (cGType_ == inspire_block){
            noteNew_->initNote(PlayWindow::initSpeed,1,composerPos_);
        } else{
            noteNew_->initNote(PlayWindow::initSpeed,0,composerPos_);
        }
        PlayWindow::setNoteToScene(noteNew_);
    }
}


