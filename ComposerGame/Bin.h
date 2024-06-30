#ifndef BIN_H
#define BIN_H

#include <QGraphicsItem>

class Bin : public QGraphicsItem
{
public:
    Bin(QPoint pos);
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    QPoint getPos() { return binPos_;}

private:
    QPoint binPos_;

};

#endif // BIN_H
