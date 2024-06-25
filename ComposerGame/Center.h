#ifndef CENTER_H
#define CENTER_H

#include <QGraphicsItem>

class Center : public QGraphicsItem
{
public:
    Center(int level, QPoint centerposition__);
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *) override;

    int getCenterWidth(){ return cWidth;}
    int getCenterHeight(){ return cHeight;}
    QPoint getCenterPos(){ return centerPos_;}

    // TODO：进入自由演奏状态/判定状态？这个晚点再说
    // void noteRecieved();

private:
    QPoint centerPos_;
    enum level{ lv_1 = 1, lv_2 } centerLv_;
    int cWidth;
    int cHeight;
};

#endif // CENTER_H
