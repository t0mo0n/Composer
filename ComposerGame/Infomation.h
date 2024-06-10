#ifndef INFOMATION_H
#define INFOMATION_H

#define IN_WIDTH 800
#define IN_HEIGHT 420

#include <QDialog>

class Infomation : public QDialog
{
    Q_OBJECT
public:
    Infomation(QWidget *parent = nullptr,int chapter=1);

protected:
    void paintEvent(QPaintEvent *ev);

private:
    void displayText();

    QPainter *infoPainter;

    int chapterInfo;
};

#endif // INFOMATION_H
