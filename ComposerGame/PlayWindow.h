#ifndef PLAYWINDOW_H
#define PLAYWINDOW_H

#define PW_WIDTH 1696
#define PW_HEIGHT 880
#define MAP_ROWS 24
#define MAP_COLS 48

#include <QDialog>
#include <QToolButton>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFile>

#include "Congratulation.h"
#include "Shop.h"
#include "Infomation.h"
#include "Block.h"
#include "Center.h"
#include "Components.h"
#include "Belt.h"
#include "Composer.h"
#include "Cutter.h"
#include "Bin.h"
#include "SpeedSwitcher.h"

namespace Ui {
class PlayWindow;
}

class PlayWindow : public QGraphicsView
{
    Q_OBJECT

public:
    explicit PlayWindow(QWidget *parent = nullptr,int chapnum = 1,QString archiveFileName = "");
    ~PlayWindow();

    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;

    static int initSpeed;
    static int composerLevel;
    static int cutterLevel;
    static enum ttype_ {belt = 1, composer, cutter, bin, speedSwitcher} toolType;

    friend Infomation;
    friend Block;
protected:
    void closeEvent(QCloseEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

signals:
    void playWindowClosed();

private slots:
    void on_ShopBtn__clicked();

    void on_InfoBtn__clicked();

    void updateGame(){this->update();}

    void on_BeltTool__clicked();

    void on_ComposerTool__clicked();

    void on_CutterTool__clicked();

    void on_BinTool__clicked();

    void on_SpeedMachineTool__clicked();

private:
    void changeStyleSheet(QToolButton *btn);
    void initMap(QFile *file = nullptr);
    void remainedAreaConstruct(const QRect& totalArea, const QList<QRect>& regularBlocks1,const QList<QRect> &regularblocks2);
    QPoint mapFromModeltoReal(QPoint sourcePos){
        return QPoint((5+sourcePos.x())*BLOCK_WIDTH,(3.5+sourcePos.y())*BLOCK_HEIGHT);
    }
    QPoint mapFromViewToGameMap(QPoint viewPos);

    Ui::PlayWindow *ui;
    QGraphicsScene *mainScene;
    QTimer *updateTimer;

    Congratulation *cGWindow;
    Shop *shopWindow;
    Infomation *infoWindow;
    Center *gameCenter;

    int bgGenerateNum;
    int chapterNum;
    Block* gameMap[MAP_ROWS][MAP_COLS];

    bool toPut;
    bool toSpin;
    bool toDelete;
    QPoint pressedPos_;
    QPoint lastPos_;

    static int Do_received;
    static int Re_received;
    static int Mi_received;
    static int Fa_received;
    static int So_received;
    static int La_received;
    static int Si_received;
    static int Empty_received;

    static QList<Belt*> belts_;
    static QList<Composer*> composers_;
    static QList<Cutter*> cutters_;
    static QList<Bin*> bins_;
    static QList<SpeedSwitcher*> speedSwitchers_;

};

#endif // PLAYWINDOW_H
