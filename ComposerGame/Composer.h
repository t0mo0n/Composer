#ifndef COMPOSER_H
#define COMPOSER_H

#include <QGraphicsObject>

class Composer : public QGraphicsObject
{
    Q_OBJECT
public:
    Composer(){};
    Composer(int level,int blockT,QPoint pos_);
    ~Composer();
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    int getDir(){ return composerDir_;}
    QPoint getPos() {return composerPos_;}
    int getLv() {return composerLv_;}
    void changeDir_(int dir);

    bool occupied;

private slots:
    void generateNote();

private:
    QPoint composerPos_;
    QTimer *generateTimer;
    enum cgBts{ normal_block = 0, inspire_block, empty_block } cGType_;
    enum direction {right = 1,down,left,up} composerDir_;
    enum levels { lv1 = 1, lv2} composerLv_;
    QPixmap composerPix;
};

#endif // COMPOSER_H
