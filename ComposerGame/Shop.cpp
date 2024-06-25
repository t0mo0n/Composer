#include "Shop.h"
#include <QWidget>
#include <QPainter>
#include <QMessageBox>
#include "EntranceWindow.h"

Shop::Shop(QWidget *parent,bool inGame)
    : QDialog(parent), ui(new Ui::Shop)
{
    ui->setupUi(this);

    this->inGame_ = inGame;
    this->setWindowTitle("Shop");
    this->setFixedSize(SH_WIDTH, SH_HEIGHT);

    shopPainter = new QPainter();

    displayText();

    changeStyleSheet(ui->smShopBtn);
    changeStyleSheet(ui->ctUpdateBtn);
    changeStyleSheet(ui->bnUpdateBtn);
    changeStyleSheet(ui->nvUpdateBtn);
}

Shop::~Shop()
{
    delete ui;
}

void Shop::paintEvent(QPaintEvent *event)
{
    shopPainter->begin(this);

    QPixmap shopBg;
    shopBg.load(":/img/resource/ShopBG.png");
    shopPainter->setOpacity(0.7);
    shopPainter->drawPixmap(0, 0, SH_WIDTH, SH_HEIGHT, shopBg);
    shopPainter->setOpacity(1.0);

    QString coinsText = "$: " + QString::number(EntranceWindow::globalState_.coins);
    shopPainter->setPen(QColor(37,139,255));
    shopPainter->setFont(QFont("Eras Bold ITC", 23));
    shopPainter->drawText(QPoint(500,46), coinsText);

    QPen shopPen = QPen(QColor(119,232,236));
    shopPen.setWidth(5);
    shopPen.setStyle(Qt::SolidLine);
    shopPainter->setPen(shopPen);
    shopPainter->drawLine(0, 70, SH_WIDTH, 70);
    shopPainter->drawLine(460, 70, 460, SH_HEIGHT);

    if (EntranceWindow::globalState_.centerLevel == 1){
        ui->ctUpdateBtn->setIcon(QIcon(":/img/resource/centerUpdate.png"));
    } else{
        ui->ctUpdateBtn->setIcon(QIcon(":/img/resource/centerUpdated.png"));
    }

    if (EntranceWindow::globalState_.noteBlockLevel == 1){
        ui->bnUpdateBtn->setIcon(QIcon(":/img/resource/musicBlockUpdate.png"));
    } else{
        ui->bnUpdateBtn->setIcon(QIcon(":/img/resource/musicBlockUpdated.png"));
    }

    if (EntranceWindow::globalState_.noteValue == 1){
        ui->nvUpdateBtn->setIcon(QIcon(":/img/resource/moreCoinsUpdate.png"));
    } else{
        ui->nvUpdateBtn->setIcon(QIcon(":/img/resource/moreCoinsUpdated.png"));
    }

    shopPainter->end();
}

void Shop::closeEvent(QCloseEvent *event)
{
    emit shopWindowClosed();
    QDialog::closeEvent(event);
}

void Shop::changeStyleSheet(QToolButton *btn)
{
    btn->setStyleSheet(
        "QToolButton { background-color: rgba(0, 0, 0, 0); border: none; }"
        "QToolButton:hover { background-color: rgba(255,255,255, 0.1); }"
        "QToolButton:pressed { background-color: rgba(255,255,255, 0.2); }"
        );
}

void Shop::displayText()
{
    QLabel *shopTitle = new QLabel(this);
    GlobalState::makeText(shopTitle,"SHOP","Eras Bold ITC",38,QColor(119,232,236),QPoint(260,10));
    shopTitle->show();

    QLabel *globalUp = new QLabel(this);
    GlobalState::makeText(globalUp,"全局强化","方正粗黑宋简体",18,QColor(37,139,255),QPoint(180,100));
    globalUp->show();

    QLabel *ctLabel_ = new QLabel(this);
    GlobalState::makeText(ctLabel_,"中心升级","方正粗黑宋简体",18,QColor(37,139,255),QPoint(46,280));
    ctLabel_->show();
    QLabel *ctPrice_ = new QLabel(this);
    GlobalState::makeText(ctPrice_,"$: 1000","Eras Bold ITC",16,QColor(37,139,255),QPoint(51,324));
    ctPrice_->show();

    QLabel *bnLabel_ = new QLabel(this);
    GlobalState::makeText(bnLabel_,"更多灵感方块","方正粗黑宋简体",16,QColor(37,139,255),QPoint(170,283));
    bnLabel_->show();
    QLabel *bnPrice_ = new QLabel(this);
    GlobalState::makeText(bnPrice_,"$: 500","Eras Bold ITC",16,QColor(37,139,255),QPoint(186,324));
    bnPrice_->show();

    QLabel *nvLabel_ = new QLabel(this);
    GlobalState::makeText(nvLabel_,"更高价值","方正粗黑宋简体",18,QColor(37,139,255),QPoint(310,280));
    nvLabel_->show();
    QLabel *nvPrice_ = new QLabel(this);
    GlobalState::makeText(nvPrice_,"$: 2000","Eras Bold ITC",16,QColor(37,139,255),QPoint(315,324));
    nvPrice_->show();

    QLabel *localUp = new QLabel(this);
    GlobalState::makeText(localUp,"局内强化","方正粗黑宋简体",18,QColor(37,139,255),QPoint(500,100));
    localUp->show();

    QLabel *smLabel_ = new QLabel(this);
    GlobalState::makeText(smLabel_,"购买变速器","方正粗黑宋简体",18,QColor(37,139,255),QPoint(490,280));
    smLabel_->show();
    QLabel *smPrice_ = new QLabel(this);
    GlobalState::makeText(smPrice_,"$: 50","Eras Bold ITC",16,QColor(37,139,255),QPoint(516,324));
    smPrice_->show();
}



void Shop::on_ctUpdateBtn_clicked()
{
    int &shop_ctLv = EntranceWindow::globalState_.centerLevel;
    if (shop_ctLv == 1){
        //判断钱够不够，如果够，花钱然后提升数值，改变图标；不够则提示
        int &shop_coins = EntranceWindow::globalState_.coins;
        if (shop_coins >= PRICE_1){
            shop_coins -= PRICE_1;
            shop_ctLv = 2;
            ui->ctUpdateBtn->setIcon(QIcon(":/img/resource/centerUpdated.png"));
            update();
        } else{
            QMessageBox::warning(this,"购买提示","你的钱不够！多赚点再来");
        }
    } else {
        QMessageBox::information(this,"购买提示","你已经升过级了");
    }
}


void Shop::on_bnUpdateBtn_clicked()
{
    int &shop_nbLv = EntranceWindow::globalState_.noteBlockLevel;
    if (shop_nbLv == 1){
        int &shop_coins = EntranceWindow::globalState_.coins;
        if (shop_coins >= PRICE_2){
            shop_coins -= PRICE_2;
            shop_nbLv = 2;
            ui->bnUpdateBtn->setIcon(QIcon(":/img/resource/musicBlockUpdated.png"));
            update();
        } else{
            QMessageBox::warning(this,"购买提示","你的钱不够！多赚点再来");
        }
    } else {
        QMessageBox::information(this,"购买提示","你已经升过级了");
    }
}


void Shop::on_nvUpdateBtn_clicked()
{
    int &shop_nvLv = EntranceWindow::globalState_.noteValue;
    if (shop_nvLv == 1){
        int &shop_coins = EntranceWindow::globalState_.coins;
        if (shop_coins >= PRICE_3){
            shop_coins -= PRICE_3;
            shop_nvLv = 2;
            ui->nvUpdateBtn->setIcon(QIcon(":/img/resource/moreCoinsUpdated.png"));
            update();
        } else{
            QMessageBox::warning(this,"购买提示","你的钱不够！多赚点再来");
        }
    } else {
        QMessageBox::information(this,"购买提示","你已经升过级了");
    }
}


void Shop::on_smShopBtn_clicked()
{
    if (inGame_){
        // 判断钱够不够，如果够，花钱，购入一个变速箱，并实时更新PlayWindow的本地信息中的变速箱数量，实时更新钱的数值；如果不够提示
        int &ssNum = PlayWindow::speedSwitcherNum;
        int &shop_coins = EntranceWindow::globalState_.coins;
        if (shop_coins >= PRICE_4){
            shop_coins -= PRICE_4;
            ssNum += 1;
            update();
        } else{
            QMessageBox::warning(this,"购买提示","你的钱不够！多赚点再来");
        }
    } else{
        QMessageBox::information(this,"购买提示","请进入游戏后再购买局部强化物品");
    }
}


