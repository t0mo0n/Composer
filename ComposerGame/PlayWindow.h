#ifndef PLAYWINDOW_H
#define PLAYWINDOW_H

#define PW_WIDTH 1696
#define PW_HEIGHT 880

#include <QWidget>
#include <QDialog>
#include <QToolButton>
#include "Congratulation.h"
#include "Shop.h"
#include "Infomation.h"

namespace Ui {
class PlayWindow;
}

class PlayWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PlayWindow(QWidget *parent = nullptr);
    ~PlayWindow();

    static int initSpeed;
    static int composerLevel;
    static int cutterLevel;

    static int Do_received;
    static int Re_received;
    static int Mi_received;
    static int Fa_received;
    static int So_received;
    static int La_received;
    static int Si_received;
    static int Empty_received;


protected:
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *event);

signals:
    void playWindowClosed();

private slots:
    void on_ShopBtn__clicked();

    void on_InfoBtn__clicked();

private:
    void changeStyleSheet(QToolButton *btn);

    Ui::PlayWindow *ui;
    QPainter *pwPainter;

    Congratulation *cGWindow;
    Shop *shopWindow;
    Infomation *infoWindow;

    int bgGenerateNum;
    int chapterNum;

};

#endif // PLAYWINDOW_H
