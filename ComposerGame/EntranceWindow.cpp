#include "EntranceWindow.h"
#include "ui_EntranceWindow.h"
#include <QPainter>
#include <QLabel>
#include<QDebug>
#include <QFontDatabase>

EntranceWindow::EntranceWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EntranceWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/img/resource/Icon.png"));
    this->setWindowTitle("Composer~Welcome!");
    this->setFixedSize(EW_WIDTH,EW_HEIGHT);

    globalState_ = GlobalState(); // 初始化全局状态，后续可能要修改为依靠配置文件改变全局状态

    entrancePainter = new QPainter();

    displayText();

    changeStyleSheet(ui->ShopButton_);
    changeStyleSheet(ui->HelpButton_);
    changeStyleSheet(ui->MusicButton_);
    changeStyleSheet(ui->SaveButton_);
    changeStyleSheet(ui->StartButton_);
}

EntranceWindow::~EntranceWindow()
{
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
    GlobalState::makeText(bgmText,"Inspiration","Eras Medium ITC",46,QColor(255,255,255),QPoint(144,699));
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
    playWindow_->show();
    this->hide();
}

void EntranceWindow::on_PlayWindowClosed()
{
    this->show();
}

