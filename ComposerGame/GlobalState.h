#ifndef GLOBALSTATE_H
#define GLOBALSTATE_H

#include <QString>
#include <QLabel>

class GlobalState{
public:
    int coins=100;
    int centerLevel=1;
    int initSpeed=1;
    int noteBlockLevel=1;

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
