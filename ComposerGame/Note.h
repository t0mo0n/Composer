#ifndef NOTE_H
#define NOTE_H

#include <QGraphicsItem>

class Belt;

class Note : public QGraphicsItem
{
public:
    Note(); // 得有默认初始化函数
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    // void noteMove();
    // 有一个指针指示它的当前绑定的通道，这个通道只会是belt类型的。别的两个生成类刚生成的音符我们不会显示
    // 至于ss，显示上我们会选择直接跳过它，所以音符的绑定通道指针我们只用考虑belt类型；
    // 当然，与此同时，面对音符的移动以及它与别的组件的交互，我们结合下一处的位置，和对应方块的内容来取出组件，并进行交互

private:
    enum types{ Empty = 0,Do,Re,Mi,Fa,So,La,Si } noteType;
    QPoint notePos_;
    int noteSpeed;
    QSharedPointer<Belt> boundedBelt;

};

#endif // NOTE_H
