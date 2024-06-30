#ifndef ENTRANCEWINDOW_H
#define ENTRANCEWINDOW_H

#define EW_WIDTH 1318
#define EW_HEIGHT 892

#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QMediaPlayer>
#include <QAudioOutput>
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

    void on_PlayWindowClosed(int c,bool finished);
    void on_UpdateFinished(int c);

    void on_SaveButton__clicked();

private:
    void changeStyleSheet(QToolButton *btn);
    void displayText();

    Ui::EntranceWindow *ui;
    QPainter* entrancePainter;
    QMediaPlayer *entranceBGM;
    QAudioOutput *eBgmOutput;

    PlayWindow *playWindow_;
    Shop *shopWindow_;
    Congratulation *cgWindow_;

};
#endif // ENTRANCEWINDOW_H
