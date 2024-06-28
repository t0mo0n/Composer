#ifndef NOTE_H
#define NOTE_H

#define NOTE_WIDTH 20
#define NOTE_HEIGHT 20

#include <QGraphicsItem>

class PlayWindow;

class Note : public QGraphicsItem
{
public:
    Note(); // 得有默认初始化函数
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    void initNote(int s, int type,QPoint _pos);

    void changeSpeed();
    void setNoteX(int x) { setPos(x,this->pos().y());}
    void setNoteY(int y) { setPos(this->pos().x(),y);}
    void moveBy(qreal dx,qreal dy); // 顺便改变速度方向
    void noteDiscard();
    // 有一个指针指示它的当前绑定的通道，这个通道只会是belt类型的。别的两个生成类刚生成的音符我们不会显示
    // 至于ss，显示上我们会选择直接跳过它，所以音符的绑定通道指针我们只用考虑belt类型；
    // 当然，与此同时，面对音符的移动以及它与别的组件的交互，我们结合下一处的位置，和对应方块的内容来取出组件，并进行交互

    friend PlayWindow;
private:
    enum types{ Empty = 0,Do,Re,Mi,Fa,So,La,Si } noteType;  
    int noteSpeed_;
    enum direction {right = 1,down,left,up} speedDir_;
    QPoint notePos_; // 真实位置
    QPoint boundedPos_; // 绑定到的位置，就取真实位置吧
    bool stop; // 移动逻辑中的分支控制标识符
    int typo_; // 用于分类寻找当前绑定的组件类型的标识符
};

#endif // NOTE_H
