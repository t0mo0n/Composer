#include "Center.h"
#include "Block.h"
#include <QPainter>
#include <QSvgRenderer>
#include <QTimer>
#include "GlobalState.h"
#include "PlayWindow.h"

Center::Center(int level, QPoint centerposition__,int chapNum) {
    playing = false;
    centerLv_ = (enum level)level;
    centerPos_ = centerposition__;
    chapterNum = chapNum;

    if (centerLv_ == lv_2){
        cWidth = 7;
        cHeight = 7;
    } else{
        cWidth = 5;
        cHeight = 5;
    }

    centerPlayer = new QMediaPlayer(this);
    notePlayer = new QMediaPlayer(this);
    cpOutput = new QAudioOutput(this);
    npOutput = new QAudioOutput(this);
    centerPlayer->setAudioOutput(cpOutput);
    notePlayer->setAudioOutput(npOutput);
    connect(notePlayer, &QMediaPlayer::playbackStateChanged, this, &Center::onNotePlayFinished);
}

Center::~Center()
{
    centerPlayer->stop();
    delete centerPlayer;
    notePlayer->stop();
    delete notePlayer;
    notePlayList.clear();
}

QRectF Center::boundingRect() const
{
    if (centerLv_ == lv_2){
        return QRectF(centerPos_.x(),centerPos_.y(),7*BLOCK_WIDTH,7*BLOCK_HEIGHT);
    } else{
        return QRectF(centerPos_.x(),centerPos_.y(),5*BLOCK_WIDTH,5*BLOCK_HEIGHT);
    }
}

void Center::paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget)
{
    if (chapterNum == 4){
        QPixmap centerPix;
        centerPix.load(":/img/resource/Center_1.png");
        painter->drawPixmap(boundingRect(),centerPix,centerPix.rect());
    } else{
        QString svgPath;
        switch (chapterNum) {
        case 1:
            svgPath = ":/img/resource/Center_Chap1.svg";
            break;
        case 2:
            svgPath = ":/img/resource/Center_Chap2.svg";
            break;
        case 3:
            svgPath = ":/img/resource/Center_Chap3.svg";
            break;
        default:
            break;
        }
        QSvgRenderer svgRenderer(svgPath);
        svgRenderer.render(painter, boundingRect());
    }
}

bool Center::chapterFinished()
{
    switch (chapterNum) {
    case 1:
        if (PlayWindow::Mi_received >= 20 && PlayWindow::Fa_received >= 20 && PlayWindow::So_received >= 40) return true;
        break;
    case 2:
        if (PlayWindow::Do_received >= 20 && PlayWindow::Mi_received >= 30 && PlayWindow::La_received >= 30 && PlayWindow::So_received >= 30) return true;
        break;
    case 3:
        if (PlayWindow::Do_received >= 30 && PlayWindow::Mi_received >= 30 && PlayWindow::So_received >= 30
            && PlayWindow::La_received >= 60 && PlayWindow::Si_received >= 40) return true;
        break;
    default:
        break;
    }
    return false;
}

void Center::noteRecieved(int noteType_)
{
    int dc = GlobalState::noteValue;
    switch (noteType_) {
    case 0:
        dc = 4*dc;
        PlayWindow::Empty_received += 1;
        break;
    case 1:
        dc = 2*dc;
        PlayWindow::Do_received += 1;
        break;
    case 2:
        dc = 4*dc;
        PlayWindow::Re_received += 1;
        break;
    case 3:
        dc = 4*dc;
        PlayWindow::Mi_received += 1;
        break;
    case 4:
        dc = 8*dc;
        PlayWindow::Fa_received += 1;
        break;
    case 5:
        dc = 8*dc;
        PlayWindow::So_received += 1;
        break;
    case 6:
        dc = 8*dc;
        PlayWindow::La_received += 1;
        break;
    case 7:
        dc = 8*dc;
        PlayWindow::Si_received += 1;
        break;
    }
    GlobalState::coins += dc;

    addNoteToPlaylist(noteType_);
}

void Center::centerPlay(int chap)
{
    if (playing){
        centerPlayer->stop();
        playing = false;
    } else{
        switch (chap) {
        case 1:
            // 534 534 5567 1234
            // https://tw.everyonepiano.com/Number-8499-1.html
            centerPlayer->setSource(QUrl("qrc:/music/resource/Canon-and-Gigue-in-D-major.mp3"));
            break;
        case 2:
            // 651 35 653 16 326 65
            // https://www.gangqinpu.com/jianpu/984835.htm
            centerPlayer->setSource(QUrl("qrc:/music/resource/The-seasons-November-Troika.mp3"));
            break;
        case 3:
            // 7656 1 2171 3 4323 7656 7656 :| 1 61 7656 76 56 765
            // https://www.qinyipu.com/jianpu/chunyinle/2241.html
            centerPlayer->setSource(QUrl("qrc:/music/resource/Rondo-Alla-Turca-piano-sonata-no.mp3"));
            break;
        default:
            // 完成了前三章，进入自由演奏模式！应该没有效果，出口在前面设置了！应该不会进入这种情况
            break;
        }
        centerPlayer->play();
        playing = true;
        connect(centerPlayer,&QMediaPlayer::playbackStateChanged,this,[this](QMediaPlayer::PlaybackState s){
            if (s == QMediaPlayer::StoppedState) {
                emit playEnd();}
        });
    }
}

void Center::addNoteToPlaylist(int noteType)
{
    QString new_note;
    switch (noteType) {
    case 0:
        new_note = "qrc:/music/resource/Empty.mp3";
        break;
    case 1:
        new_note = "qrc:/music/resource/Do.m4a";
        break;
    case 2:
        new_note = "qrc:/music/resource/Re.m4a";
        break;
    case 3:
        new_note = "qrc:/music/resource/Mi.m4a";
        break;
    case 4:
        new_note = "qrc:/music/resource/Fa.m4a";
        break;
    case 5:
        new_note = "qrc:/music/resource/Sol.m4a";
        break;
    case 6:
        new_note = "qrc:/music/resource/La.m4a";
        break;
    case 7:
        new_note = "qrc:/music/resource/Si.m4a";
        break;
    }
    notePlayList.append(new_note);
    if(notePlayer->playbackState() != QMediaPlayer::PlayingState) { // 如果音频播放器不在播放状态
        playNextNote();
    }
}

void Center::playNextNote()
{
    if (!notePlayList.isEmpty()) {
        QString nextAudio = notePlayList.takeFirst(); // 移除并返回列表中的第一个音频路径
        notePlayer->stop(); // 停止当前播放
        notePlayer->setSource(QUrl()); // 清除当前源
        notePlayer->setSource(QUrl(nextAudio)); // 设置新的源
        notePlayer->play(); // 开始播放
        // 使用QTimer在一段时间后停止播放
        QTimer::singleShot(750, notePlayer, &QMediaPlayer::stop);
    }
}

void Center::onNotePlayFinished()
{
    if (notePlayer->playbackState() != QMediaPlayer::PlayingState) { // 如果音频播放器播放完毕(与PlayingState的stoppedState相对应)
        playNextNote();
    }
}

