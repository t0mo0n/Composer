#ifndef SHOP_H
#define SHOP_H

#define SH_WIDTH 640
#define SH_HEIGHT 420
#define PRICE_1 1000
#define PRICE_2 500
#define PRICE_3 2000
#define PRICE_4 50

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
    Shop(QWidget *parent = nullptr, bool inGame = false);
    ~Shop();
protected:
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void changeStyleSheet(QToolButton *btn);
    void displayText();

signals:
    void shopWindowClosed();

private slots:
    void on_ctUpdateBtn_clicked();
    void on_bnUpdateBtn_clicked();
    void on_nvUpdateBtn_clicked();
    void on_smShopBtn_clicked();

private:
    Ui::Shop *ui;

    QPainter *shopPainter;
    bool inGame_;
};

#endif // SHOP_H
