#ifndef SPEEDSWITCHER_H
#define SPEEDSWITCHER_H

#include <QGraphicsItem>

class SpeedSwitcher : public QGraphicsItem
{
public:
    SpeedSwitcher(QPoint pos);
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    QPoint getPos() { return ssPos_;}
    int getDir(){ return ssDir_;}
    void changeDir_(int dir);

    bool occupied;
private:
    QPoint ssPos_;
    enum direction {right = 1,down,left,up} ssDir_;
    QPixmap ssPix;
};

#endif // SPEEDSWITCHER_H
