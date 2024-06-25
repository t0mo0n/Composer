#ifndef COMPOSER_H
#define COMPOSER_H

#include <QGraphicsItem>

class Composer : public QGraphicsItem
{
public:
    Composer(int level,QPoint pos_);
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    int getDir(){ return composerDir_;}
    void changeDir_(int dir);
    // void generateNote(); // TODO
    bool occupied;

private:
    QPoint composerPos_;
    enum direction {right = 1,down,left,up} composerDir_;
    enum levels { lv1 = 1, lv2} composerLv_;
    QPixmap composerPix;
};

#endif // COMPOSER_H
