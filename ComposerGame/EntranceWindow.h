#ifndef ENTRANCEWINDOW_H
#define ENTRANCEWINDOW_H

#define EW_WIDTH 1318
#define EW_HEIGHT 892

#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include "GlobalState.h"
#include "Shop.h"
#include "PlayWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class EntranceWindow;
}
QT_END_NAMESPACE

class EntranceWindow : public QWidget
{
    Q_OBJECT

public:
    static GlobalState globalState_;

    EntranceWindow(QWidget *parent = nullptr);
    ~EntranceWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_ShopButton__clicked();
    void on_StartButton__clicked();
    void on_PlayWindowClosed();

private:
    void changeStyleSheet(QToolButton *btn);
    void displayText();

    Ui::EntranceWindow *ui;
    QPainter* entrancePainter;

    PlayWindow *playWindow_;
    Shop *shopWindow_;
};
#endif // ENTRANCEWINDOW_H
