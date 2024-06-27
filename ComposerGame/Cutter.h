#ifndef CUTTER_H
#define CUTTER_H

#include <QGraphicsItem>

class Cutter : public QGraphicsItem
{
public:
    Cutter(int level,QPoint pos_);
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    int getDir(){ return cutterDir_;}
    void changeDir_(int dir);
    QPoint getPos_1(){return cutterPos_1;}
    QPoint getPos_2(){return cutterPos_2;}

    void generateNote(int noteCuttedType);

    bool occupied;

private:
    QPoint cutterPos_1;
    QPoint cutterPos_2;
    enum direction {right = 1,down,left,up} cutterDir_; // 出口方向
    enum levels { lv1 = 1, lv2} cutterLv_;
    QPixmap cutterPix;
};

#endif // CUTTER_H
