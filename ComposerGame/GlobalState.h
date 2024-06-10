#ifndef GLOBALSTATE_H
#define GLOBALSTATE_H

#include <QString>
#include <QLabel>
#include <QWidget>

class GlobalState
{
public:
    // 好吧我承认我的这个类没有实现的必要，可以合并到EntranceWindow里
    GlobalState();

    static int coins;
    static int centerLevel;
    static int noteBlockLevel;
    static int noteValue;

    static void makeText(QLabel *label,const QString &text,const QString &font, int size, QColor color, QPoint textPos){
        label->setText(text);
        QFont Font;
        Font.setFamily(font);
        Font.setPointSize(size);
        label->setFont(Font);
        QPalette titlePalette;
        titlePalette.setColor(QPalette::WindowText, color);
        label->setPalette(titlePalette);
        label->move(textPos);
    };
};


#endif // GLOBALSTATE_H
