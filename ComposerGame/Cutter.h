#ifndef CUTTER_H
#define CUTTER_H

#include <QGraphicsObject>

class Cutter : public QGraphicsObject
{
    Q_OBJECT
public:
    Cutter(int level,QPoint pos_);
    ~Cutter();
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    int getDir(){ return cutterDir_;}
    void changeDir_(int dir);
    QPoint getPos_1(){return cutterPos_1;}
    QPoint getPos_2(){return cutterPos_2;}
    int getLv() {return cutterLv_;}

    void generateNote(int noteCuttedType,int v);

    int occupied;

private slots:
    void generateNote_slot();

private:
    QPoint cutterPos_1;
    QPoint cutterPos_2;
    QTimer *generateTimer;
    enum direction {right = 1,down,left,up} cutterDir_; // 出口方向
    enum levels { lv1 = 1, lv2} cutterLv_;
    int noteToCutType;
    int noteInitSpeed;
    QPixmap cutterPix;
};

#endif // CUTTER_H
