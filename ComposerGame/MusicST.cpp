#include "MusicST.h"
#include <QLabel>
#include "GlobalState.h"
#include "ui_MusicST.h"
#include <QPainter>

MusicST::MusicST(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MusicST)
{
    ui->setupUi(this);

    this->setWindowTitle("MST");
    this->setWindowIcon(QIcon(":/img/resource/Play.png"));
    this->setFixedSize(MST_WIDTH,MST_HEIGHT);

    MST_painter = new QPainter();
    playing_ = false;
    MST_player = new QMediaPlayer(this);
    MST_Output = new QAudioOutput(this);
    MST_Output->setVolume(1.0);
    MST_player->setAudioOutput(MST_Output);

    connect(MST_player,&QMediaPlayer::playbackStateChanged,this,[this](QMediaPlayer::PlaybackState s){
        if (s == QMediaPlayer::StoppedState){
            playing_ = false;
        }
    });

    QLabel *entranceBGM_1 = new QLabel(this);
    GlobalState::makeText(entranceBGM_1, "Arabesque-No1 阿拉伯风格曲 德彪西","等线 Light",20,QColor(0,0,0),QPoint(54,64));
    entranceBGM_1->show();

    QLabel *playBGM_1 = new QLabel(this);
    GlobalState::makeText(playBGM_1,"My-love Taylor-Anthony","等线 Light",20,QColor(0,0,0),QPoint(66,168));
    playBGM_1->show();

    QLabel *playBGM_2 = new QLabel(this);
    GlobalState::makeText(playBGM_2,"If-I-Can-Stop-One-Heart-From-Breaking 知更鸟", "等线 Light",20,QColor(0,0,0),QPoint(48,269));
    playBGM_2->show();

    QLabel *playBGM_3 = new QLabel(this);
    GlobalState::makeText(playBGM_3,"Florence 佛罗伦萨 Kevin-Penkin", "等线 Light",20,QColor(0,0,0),QPoint(54,380));
    playBGM_3->show();

    QLabel *chapterBGM_1 = new QLabel(this);
    GlobalState::makeText(chapterBGM_1,"Canon-in-D-major D大调卡农 帕赫贝尔","等线 Light",20,QColor(255,255,255),QPoint(310,110));
    chapterBGM_1->show();

    QLabel *chapterBGM_2 = new QLabel(this);
    GlobalState::makeText(chapterBGM_2,"November-Troika 十一月-雪橇 柴可夫斯基","等线 Light",20,QColor(255,255,255),QPoint(302,220));
    chapterBGM_2->show();

    QLabel *chapterBGM_3 = new QLabel(this);
    GlobalState::makeText(chapterBGM_3,"Turca-piano-sonata 土耳其进行曲 莫扎特","等线 Light",20,QColor(255,255,255),QPoint(298,330));
    chapterBGM_3->show();

    // QLabel *bgm_hint = new QLabel(this);
    // GlobalState::makeText(bgm_hint,"bgms","等线 Light",32,QColor(0,0,0),QPoint(64,500));
    // bgm_hint->show();

    // QLabel *chapter_hint = new QLabel(this);
    // GlobalState::makeText(chapter_hint,"chapters","等线 Light",32,QColor(255,255,255),QPoint(600,500));
    // chapter_hint->show();

    QLabel *title = new QLabel(this);
    GlobalState::makeText(title,"Composer 游戏音乐欣赏","等线 Light",30,QColor(0,0,0),QPoint(220,505));
    title->show();

    changeStyleSheet(ui->tB_1);
    changeStyleSheet(ui->tB_2);
    changeStyleSheet(ui->tB_3);
    changeStyleSheet(ui->tB_4);
    changeStyleSheet(ui->tB_5);
    changeStyleSheet(ui->tB_6);
    changeStyleSheet(ui->tB_7);
}

MusicST::~MusicST()
{
    delete ui;
}

void MusicST::paintEvent(QPaintEvent *event)
{
    MST_painter->begin(this);
    QPixmap MST_bg;
    MST_bg.load(":/img/resource/musicST_BG.png");
    MST_painter->drawPixmap(0,0,MST_WIDTH,MST_HEIGHT,MST_bg);
    MST_painter->end();
}

void MusicST::closeEvent(QCloseEvent *event)
{
    if (playing_) {
        MST_player->stop();
        playing_ = false;
    };
    // delete MST_player;
    // MST_player = nullptr;
    // delete MST_Output;
    // MST_Output = nullptr;
    emit mstWindowClosed();
    QDialog::closeEvent(event);
}

void MusicST::changeStyleSheet(QToolButton *btn)
{
    btn->setStyleSheet(
        "QToolButton { background-color: rgba(0, 0, 0, 0); border: none; }"
        "QToolButton:hover { background-color: rgba(0,0,0, 0.1); }"
        "QToolButton:pressed { background-color: rgba(0,0,0, 0.2); }"
        );
}

void MusicST::on_tB_1_clicked()
{
    if (playing_){
        MST_player->stop();
        MST_player->setSource(QUrl());
        MST_player->setSource(QUrl("qrc:/music/resource/Arabesque-No1-Andantino-con-moto.mp3"));
        MST_player->play();
        playing_ = true;
    } else {
        MST_player->setSource(QUrl("qrc:/music/resource/Arabesque-No1-Andantino-con-moto.mp3"));
        MST_player->play();
        playing_ = true;
    }
}


void MusicST::on_tB_2_clicked()
{
    if (playing_){
        MST_player->stop();
        MST_player->setSource(QUrl());
        MST_player->setSource(QUrl("qrc:/music/resource/Canon-and-Gigue-in-D-major.mp3"));
        MST_player->play();
        playing_ = true;
    } else {
        MST_player->setSource(QUrl("qrc:/music/resource/Canon-and-Gigue-in-D-major.mp3"));
        MST_player->play();
        playing_ = true;
    }
}


void MusicST::on_tB_3_clicked()
{
    if (playing_){
        MST_player->stop();
        MST_player->setSource(QUrl());
        MST_player->setSource(QUrl("qrc:/music/resource/My-love.mp3"));
        MST_player->play();
        playing_ = true;
    } else {
        MST_player->setSource(QUrl("qrc:/music/resource/My-love.mp3"));
        MST_player->play();
        playing_ = true;
    }
}


void MusicST::on_tB_4_clicked()
{
    if (playing_){
        MST_player->stop();
        MST_player->setSource(QUrl());
        MST_player->setSource(QUrl("qrc:/music/resource/The-seasons-November-Troika.mp3"));
        MST_player->play();
        playing_ = true;
    } else {
        MST_player->setSource(QUrl("qrc:/music/resource/The-seasons-November-Troika.mp3"));
        MST_player->play();
        playing_ = true;
    }
}


void MusicST::on_tB_5_clicked()
{
    if (playing_){
        MST_player->stop();
        MST_player->setSource(QUrl());
        MST_player->setSource(QUrl("qrc:/music/resource/If-I-Can-Stop-One-Heart-From-Breaking.mp3"));
        MST_player->play();
        playing_ = true;
    } else {
        MST_player->setSource(QUrl("qrc:/music/resource/If-I-Can-Stop-One-Heart-From-Breaking.mp3"));
        MST_player->play();
        playing_ = true;
    }
}


void MusicST::on_tB_6_clicked()
{
    if (playing_){
        MST_player->stop();
        MST_player->setSource(QUrl());
        MST_player->setSource(QUrl("qrc:/music/resource/Rondo-Alla-Turca-piano-sonata-no.mp3"));
        MST_player->play();
        playing_ = true;
    } else {
        MST_player->setSource(QUrl("qrc:/music/resource/Rondo-Alla-Turca-piano-sonata-no.mp3"));
        MST_player->play();
        playing_ = true;
    }
}


void MusicST::on_tB_7_clicked()
{
    if (playing_){
        MST_player->stop();
        MST_player->setSource(QUrl());
        MST_player->setSource(QUrl("qrc:/music/resource/Florence.mp3"));
        MST_player->play();
        playing_ = true;
    } else {
        MST_player->setSource(QUrl("qrc:/music/resource/Florence.mp3"));
        MST_player->play();
        playing_ = true;
    }
}

