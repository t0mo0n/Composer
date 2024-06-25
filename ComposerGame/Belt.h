#ifndef BELT_H
#define BELT_H

#include <QGraphicsItem>

class Belt : public QGraphicsItem
{
public:
    Belt(){
        // occupied = false;
    }
    void initBelt(int initDir, QPoint pos);
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    void deleteBelt();

    int getDir(){ return beltDir_;}
    void changeDir_(int dir);

    bool occupied;

private:
    enum direction{LeftToRight = 1, RightToLeft,UpToDown,DownToUp,LeftToUp,RightToUp,UpToLeft,DownToLeft,
                     LeftToDown,RightToDown,UpToRight,DownToRight} beltDir_;
    QPoint beltPos_;
    QPixmap beltPix;

};

#endif // BELT_H
