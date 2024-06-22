#ifndef BLOCK_H
#define BLOCK_H

#define BLOCK_WIDTH 32
#define BLOCK_HEIGHT 32

#include <QWidget>
#include <QGraphicsItem>

class Block : public QGraphicsItem
{
public:
    Block(int type);

    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    // void mousePressEvent(QGraphicsSceneMouseEvent *) override;
    // void mouseReleaseEvent(QGraphicsSceneMouseEvent *) override;

    void setBlockPos_(QPoint pos);
    QPoint &getBlockPos_();
    bool occupied;
private:
    enum blockType_{ normal_block = 0, inspire_block, empty_block } bType;
    QPoint blockPos_;
    // bool pressed;

    // PlayWindow *myPw_;
};

#endif // BLOCK_H
