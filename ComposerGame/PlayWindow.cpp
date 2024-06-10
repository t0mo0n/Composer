#include "PlayWindow.h"
#include "ui_PlayWindow.h"
#include <QPainter>
#include <QRandomGenerator>

int PlayWindow::initSpeed=1;
int PlayWindow::composerLevel=1;
int PlayWindow::cutterLevel=1;
int PlayWindow::Do_received=0;
int PlayWindow::Re_received=0;
int PlayWindow::Mi_received=0;
int PlayWindow::Fa_received=0;
int PlayWindow::So_received=0;
int PlayWindow::La_received=0;
int PlayWindow::Si_received=0;
int PlayWindow::Empty_received=0;

PlayWindow::PlayWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/img/resource/Icon.png"));
    this->resize(PW_WIDTH,PW_HEIGHT);
    this->setWindowTitle("Let's Play!");

    pwPainter = new QPainter(this);

    bgGenerateNum = QRandomGenerator::global()->bounded(1, 4);
    chapterNum = 1; //默认值，可以通过导入存档来更改

    changeStyleSheet(ui->ShopBtn_);
    changeStyleSheet(ui->InfoBtn_);
    changeStyleSheet(ui->SaveBtn_);
    changeStyleSheet(ui->HelpBtn_);

    changeStyleSheet(ui->BeltTool_);
    changeStyleSheet(ui->ComposerTool_);
    changeStyleSheet(ui->CutterTool_);
    changeStyleSheet(ui->BinTool_);
    changeStyleSheet(ui->SpeedMachineTool_);

    // 测试通关界面：
    // cGWindow = new Congratulation(this);
    // cGWindow->exec();
}

PlayWindow::~PlayWindow()
{
    delete ui;
}

void PlayWindow::closeEvent(QCloseEvent *event)
{
    emit playWindowClosed();
    QWidget::closeEvent(event);
}

void PlayWindow::paintEvent(QPaintEvent *event)
{
    pwPainter->begin(this);

    QPixmap pwBg;

    switch (bgGenerateNum) {
    case 1:
        pwBg.load(":/img/resource/PlayWindowBg.png");
        break;
    case 2:
        pwBg.load(":/img/resource/PlayWindowBg2.png");
        break;
    case 3:
        pwBg.load(":/img/resource/PlayWindowBg3.png");
        break;
    }
    pwPainter->setOpacity(0.33);
    pwPainter->drawPixmap(0, 0, PW_WIDTH,PW_HEIGHT, pwBg);
    pwPainter->setOpacity(1.0);

    pwPainter->end();
}

void PlayWindow::changeStyleSheet(QToolButton *btn)
{
    btn->setStyleSheet(
        "QToolButton { background-color: rgba(0, 0, 0, 0); border: none; }"
        "QToolButton:hover { background-color: rgba(0,0,0, 0.1); }"
        "QToolButton:pressed { background-color: rgba(0,0,0, 0.2); }"
        );
}

void PlayWindow::on_ShopBtn__clicked()
{
    shopWindow = new Shop(this,true);
    shopWindow->exec();
}


void PlayWindow::on_InfoBtn__clicked()
{
    infoWindow = new Infomation(this,chapterNum);
    infoWindow->exec();
}

