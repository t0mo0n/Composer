#include "Shop.h"
#include <QWidget>
#include <QPainter>
#include "GlobalState.h"

Shop::Shop(QWidget *parent)
    : QDialog(parent), ui(new Ui::Shop)
{
    ui->setupUi(this);

    this->setWindowTitle("Shop");
    this->setFixedSize(SH_WIDTH, SH_HEIGHT);

    QLabel *shopTitle = new QLabel(this);
    GlobalState::makeText(shopTitle,"SHOP","Eras Light ITC",38,QColor(119,232,236),QPoint(260,10));
    shopTitle->show();
}

Shop::~Shop()
{
    delete ui;
}

void Shop::paintEvent(QPaintEvent *event)
{
    shopPainter->begin(this);

    QPixmap shopBg;
    shopBg.load("");

    shopPainter->end();
}
