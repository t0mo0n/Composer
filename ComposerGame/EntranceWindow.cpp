#include "EntranceWindow.h"
#include "ui_EntranceWindow.h"
#include <QPainter>
#include <QLabel>
#include<QDebug>
#include <QFontDatabase>
#include <QMediaPlayer>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileDialog>

// 注意，config文件里得一开始就有基本的全局变量信息，不然程序会崩溃！这点在打包exe文件时要注意！
EntranceWindow::EntranceWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EntranceWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/img/resource/Icon.png"));
    this->setWindowTitle("Composer~Welcome!");
    this->setFixedSize(EW_WIDTH,EW_HEIGHT);

    globalState_ = GlobalState(); // 初始化全局状态
    QFile configFile("F:/advProgramProject/Composer/files/configFile.json");
    if(!configFile.open(QIODevice::ReadOnly)){
        qWarning("全局配置文件无法打开，请确认配置文件是否存在");
    } else{
        QByteArray configData = configFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(configData));
        QJsonObject configObj = loadDoc.object();
        GlobalState::coins = configObj["coins"].toInt();
        GlobalState::centerLevel = configObj["centerLv"].toInt();
        GlobalState::noteBlockLevel = configObj["noteBlockLv"].toInt();
        GlobalState::noteValue = configObj["noteValue"].toInt();
    }
    configFile.close();

    entrancePainter = new QPainter();
    entranceBGM = new QMediaPlayer(this);
    eBgmOutput = new QAudioOutput(this);
    eBgmOutput->setVolume(1);
    entranceBGM->setAudioOutput(eBgmOutput);

    displayText();

    changeStyleSheet(ui->ShopButton_);
    changeStyleSheet(ui->HelpButton_);
    changeStyleSheet(ui->MusicButton_);
    changeStyleSheet(ui->SaveButton_);
    changeStyleSheet(ui->StartButton_);

    entranceBGM->setSource(QUrl("qrc:/music/resource/Arabesque-No1-Andantino-con-moto.mp3"));
    entranceBGM->setLoops(-1);
    entranceBGM->play();
}

EntranceWindow::~EntranceWindow()
{
    QFile configFile("F:/advProgramProject/Composer/files/configFile.json");
    if(!configFile.open(QIODevice::WriteOnly)){
        qWarning("全局配置文件无法打开，请确认配置文件是否存在");
        return;
    }
    QJsonObject configData;
    configData["coins"] = GlobalState::coins;
    configData["centerLv"] = GlobalState::centerLevel;
    configData["noteBlockLv"] = GlobalState::noteBlockLevel;
    configData["noteValue"] = GlobalState::noteValue;
    configFile.write(QJsonDocument(configData).toJson());
    configFile.close();

    entranceBGM->stop();
    delete ui;
}

void EntranceWindow::paintEvent(QPaintEvent *event)
{
    entrancePainter->begin(this);
    entrancePainter->drawImage(QRect(0,0,EW_WIDTH,EW_HEIGHT),QImage(":/img/resource/EntranceBackground.png"));
    entrancePainter->end();
}

void EntranceWindow::changeStyleSheet(QToolButton *btn)
{
    btn->setStyleSheet(
        "QToolButton { background-color: rgba(0, 0, 0, 0); border: none; }"
        "QToolButton:hover { background-color: rgba(0,0,0, 0.1); }"
        "QToolButton:pressed { background-color: rgba(0,0,0, 0.2); }"
        );
}

void EntranceWindow::displayText()
{
    QLabel* mainTitle = new QLabel(this);
    GlobalState::makeText(mainTitle,"Composer","Harlow Solid Italic",105,QColor(225,127,15),QPoint(661,153));
    mainTitle->show();

    QLabel* bgmText = new QLabel(this);
    GlobalState::makeText(bgmText,"Florence","Eras Medium ITC",46,QColor(255,255,255),QPoint(144,699));
    mainTitle->show();
}

void EntranceWindow::on_ShopButton__clicked()
{
    shopWindow_ = new Shop(this);
    shopWindow_->exec();
}

void EntranceWindow::on_StartButton__clicked()
{
    playWindow_ = new PlayWindow();
    connect(playWindow_,&PlayWindow::playWindowClosed,this,&EntranceWindow::on_PlayWindowClosed);
    entranceBGM->stop();
    playWindow_->show();
    this->hide();
}

void EntranceWindow::on_PlayWindowClosed(int c,bool finished)
{
    this->show();
    entranceBGM->play();
    if (finished){
        cgWindow_ = new Congratulation(this,c);
        connect(cgWindow_,&Congratulation::updateFinished,this,&EntranceWindow::on_UpdateFinished);
        cgWindow_->exec();
    }
}

void EntranceWindow::on_UpdateFinished(int c)
{
    playWindow_ = new PlayWindow("",c);
    connect(playWindow_,&PlayWindow::playWindowClosed,this,&EntranceWindow::on_PlayWindowClosed);
    entranceBGM->stop();
    playWindow_->show();
    this->hide();
}


void EntranceWindow::on_SaveButton__clicked()
{
    QString archiveFileName = QFileDialog::getOpenFileName(this,"读取存档","F:/advProgramProject/Composer/files/Archives");
    playWindow_ = new PlayWindow(archiveFileName);
    connect(playWindow_,&PlayWindow::playWindowClosed,this,&EntranceWindow::on_PlayWindowClosed);
    entranceBGM->stop();
    playWindow_->show();
    this->hide();
}

