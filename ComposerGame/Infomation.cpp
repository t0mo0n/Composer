#include "Infomation.h"
#include <QPainter>
#include "GlobalState.h"
#include "PlayWindow.h"


Infomation::Infomation(QWidget *parent,int chapter)
    : QDialog(parent)
{
    this->chapterInfo = chapter;

    this->setWindowTitle("本关信息");
    this->setFixedSize(IN_WIDTH,IN_HEIGHT);

    infoPainter = new QPainter(this);

    displayText();
}

void Infomation::paintEvent(QPaintEvent *ev)
{
    infoPainter->begin(this);

    QPixmap infoBg;
    infoBg.load(":/img/resource/musicSt&Info.png");
    infoPainter->drawPixmap(0,0,IN_WIDTH,IN_HEIGHT,infoBg);

    infoPainter->end();
}

void Infomation::displayText()
{
    QLabel *title = new QLabel(this);
    GlobalState::makeText(title,"关卡信息","方正姚体",32,QColor(0,0,0),QPoint(38,27));
    title->show();

    QLabel *chapterInfo_1 = new QLabel(this);
    GlobalState::makeText(chapterInfo_1,"当前关卡","方正姚体",25,QColor(0,0,0),QPoint(592,18));
    chapterInfo_1->show();

    QLabel *chapterInfo_2 = new QLabel(this);
    QString chapterName;
    switch(chapterInfo)
    {
    case 1:
        chapterName = "D大调卡农";
        break;
    case 2:
        chapterName = "在马车上";
        break;
    case 3:
        chapterName = "Florence";
        break;
    }
    GlobalState::makeText(chapterInfo_2,"第 "+QString::number(chapterInfo)+" 乐章 " + chapterName,"方正姚体",18,QColor(0,0,0),QPoint(570,75));
    chapterInfo_2->show();

    QLabel *Do_recNum = new QLabel(this);
    GlobalState::makeText(Do_recNum,"已收集 Do: "+QString::number(PlayWindow::Do_received),"方正姚体",21,QColor(0,0,0),QPoint(29,118));
    Do_recNum->show();

    QLabel *Re_recNum = new QLabel(this);
    GlobalState::makeText(Re_recNum,"已收集 Re: "+QString::number(PlayWindow::Re_received),"方正姚体",21,QColor(0,0,0),QPoint(29,175));
    Re_recNum->show();

    QLabel *Mi_recNum = new QLabel(this);
    GlobalState::makeText(Mi_recNum,"已收集 Mi: "+QString::number(PlayWindow::Mi_received),"方正姚体",21,QColor(0,0,0),QPoint(29,232));
    Mi_recNum->show();

    QLabel *Fa_recNum = new QLabel(this);
    GlobalState::makeText(Fa_recNum,"已收集 Fa: "+QString::number(PlayWindow::Fa_received),"方正姚体",21,QColor(0,0,0),QPoint(29,289));
    Fa_recNum->show();

    QLabel *So_recNum = new QLabel(this);
    GlobalState::makeText(So_recNum,"已收集 So: "+QString::number(PlayWindow::So_received),"方正姚体",21,QColor(0,0,0),QPoint(29,346));
    So_recNum->show();

    QLabel *La_recNum = new QLabel(this);
    GlobalState::makeText(La_recNum,"已收集 La: "+QString::number(PlayWindow::La_received),"方正姚体",21,QColor(0,0,0),QPoint(632,265));
    La_recNum->show();

    QLabel *Si_recNum = new QLabel(this);
    GlobalState::makeText(Si_recNum,"已收集 Si: "+QString::number(PlayWindow::Si_received),"方正姚体",21,QColor(0,0,0),QPoint(632,320));
    Si_recNum->show();

    QLabel *Empty_recNum = new QLabel(this);
    GlobalState::makeText(Empty_recNum,"已收集 空音符: "+QString::number(PlayWindow::Empty_received),"方正姚体",21,QColor(0,0,0),QPoint(572,370));
    Empty_recNum->show();

}
