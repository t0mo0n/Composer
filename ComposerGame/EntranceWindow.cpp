#include "EntranceWindow.h"
#include "ui_EntranceWindow.h"
#include <QPainter>
#include <QLabel>
#include<QDebug>
#include <QFontDatabase>
#include "GlobalState.h"
#include "Shop.h"

EntranceWindow::EntranceWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EntranceWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/img/resource/Icon.png"));
    this->setWindowTitle("Composer~Welcome!");
    this->setFixedSize(EW_WIDTH,EW_HEIGHT);

    entrancePainter = new QPainter();

    displayText();

    changeStyleSheet(ui->ShopButton_);
    changeStyleSheet(ui->HelpButton_);
    changeStyleSheet(ui->MusicButton_);
    changeStyleSheet(ui->SaveButton_);
    changeStyleSheet(ui->StartButton_);

    // ui->newGameBtn->setStyleSheet(
    //     "QPushButton { background-color: rgba(0, 0, 0, 0); border: none; }"
    //     "QPushButton:hover { background-color: rgba(255,255,255, 0.1); }"
    //     "QPushButton:pressed { background-color: rgba(255,255,255, 0.2); }"
    //     );

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

    // ------调试，查看本机字体
    // QFontDatabase database;
    // foreach (const QString &family, database.families())
    // {
    //     qDebug() << family;
    // }
}

void EntranceWindow::on_ShopButton__clicked()
{
    Shop s = Shop(this);
    s.exec();
}

