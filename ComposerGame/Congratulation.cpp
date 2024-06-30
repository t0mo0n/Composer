#include "Congratulation.h"
#include "ui_Congratulation.h"
#include <QLabel>
#include <QPainter>
#include <QMessageBox>
#include "GlobalState.h"
#include "PlayWindow.h"

Congratulation::Congratulation(QWidget *parent,int chapter)
    : QDialog(parent)
    , ui(new Ui::Congratulation)
{
    ui->setupUi(this);

    this->conChapter_ = chapter;
    this->setWindowTitle("Congratulations!");
    this->setFixedSize(CON_WIDTH,CON_HEIGHT);

    conPainter = new QPainter(this);

    QLabel *titleLabel = new QLabel(this);
    QString titleText = "恭喜完成第" + QString::number(conChapter_) + "乐章！";
    GlobalState::makeText(titleLabel,titleText,"方正粗黑宋简体",34,QColor(229,144,59),QPoint(156,95));
    titleLabel->show();

    QLabel *localUpL1_ = new QLabel(this);
    GlobalState::makeText(localUpL1_,"强化传送带\n基础速度(荐","方正粗黑宋简体",16,QColor(37,139,255),QPoint(78,300));
    localUpL1_->show();

    QLabel *localUpL2_ = new QLabel(this);
    GlobalState::makeText(localUpL2_,"强化创作方\n块创作速度","方正粗黑宋简体",16,QColor(37,139,255),QPoint(270,300));
    localUpL2_->show();

    QLabel *localUpL3_ = new QLabel(this);
    GlobalState::makeText(localUpL3_,"强化切割机\n切割速度","方正粗黑宋简体",16,QColor(37,139,255),QPoint(468,300));
    localUpL3_->show();

    QLabel *upTips_ = new QLabel(this);
    GlobalState::makeText(upTips_,"选择一项进行强化！","方正粗黑宋简体",14,QColor(37,139,255),QPoint(470,395));
    upTips_->show();

    changeStyleSheet(ui->BeltUpBtn_);
    changeStyleSheet(ui->CenterUpBtn_);
    changeStyleSheet(ui->CutterUpBtn_);
}

Congratulation::~Congratulation()
{
    delete ui;
}

void Congratulation::paintEvent(QPaintEvent *event)
{
    conPainter->begin(this);

    QPixmap consPix;
    consPix.load(":/img/resource/ShopBG.png");
    conPainter->setOpacity(0.6);
    conPainter->drawPixmap(0,0,CON_WIDTH,CON_HEIGHT,consPix);
    conPainter->setOpacity(1.0);

    consPix.load(":/img/resource/Congratulations.png");
    conPainter->drawPixmap(283,12,60,60,consPix);

    conPainter->end();
}

void Congratulation::closeEvent(QCloseEvent *event)
{
    // 检测到发出的已经完成升级的信号，自动关闭此窗口，并且结合目前的关卡情况,和全局状态，对下一关进行初始化;
    emit updateFinished(conChapter_+1);
    QDialog::closeEvent(event);
}

void Congratulation::changeStyleSheet(QToolButton *btn)
{
    btn->setStyleSheet(
        "QToolButton { background-color: rgba(0, 0, 0, 0); border: none; }"
        "QToolButton:hover { background-color: rgba(0,0,0, 0.1); }"
        "QToolButton:pressed { background-color: rgba(0,0,0, 0.2); }"
        );
}

void Congratulation::on_BeltUpBtn__clicked()
{
    auto btn = QMessageBox::information(this,"强化确认","确定要强化传送带\n基础速度（推荐）？",QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if (btn == QMessageBox::Yes){
        // 改变本局局内强化变量
        PlayWindow::initSpeed += 1;
        this->close();
    } else{
        QMessageBox::information(this,"强化确认","请选择一项强化后\n继续游戏");
    }
}


void Congratulation::on_CenterUpBtn__clicked()
{
    auto btn = QMessageBox::information(this,"强化确认","确定要强化创作\n方块创作速度？",QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if (btn == QMessageBox::Yes){
        PlayWindow::composerLevel += 1;
        this->close();
    } else{
        QMessageBox::information(this,"强化确认","请选择一项强化后\n继续游戏");
    }
}


void Congratulation::on_CutterUpBtn__clicked()
{
    auto btn = QMessageBox::information(this,"强化确认","确定要强化切割\n机切割速度？",QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if (btn == QMessageBox::Yes){
        PlayWindow::cutterLevel += 1;
        this->close();
    } else{
        QMessageBox::information(this,"强化确认","请选择一项强化后\n继续游戏");
    }
}

