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

    void setBlockPos_(QPoint pos);
    int getBlockContent(){ return bContent;}
    int getBlockType() {return bType;}
    void setBlockContent(int ct){ bContent = (enum blockContent)ct;}

    bool occupied; //  对于组件的属性
private:
    enum blockType_{ normal_block = 0, inspire_block, empty_block } bType;
    enum blockContent{ empty = 0,belt,composer,cutter,bin,speedSwitcher,center} bContent;
    QPoint blockPos_;
};

#endif // BLOCK_H
