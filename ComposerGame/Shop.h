#ifndef SHOP_H
#define SHOP_H

#define SH_WIDTH 640
#define SH_HEIGHT 420

#include "ui_Shop.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class Shop;
}
QT_END_NAMESPACE

class Shop : public QDialog
{
    Q_OBJECT
public:
    Shop(QWidget *parent = nullptr);
    ~Shop();
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::Shop *ui;

    QPainter *shopPainter;
};

#endif // SHOP_H
