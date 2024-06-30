#ifndef CENTER_H
#define CENTER_H

#include <QGraphicsObject>
#include <QMediaPlayer>
#include <QAudioOutput>

class Center : public QGraphicsObject
{
    Q_OBJECT
public:
    Center(int level, QPoint centerposition__,int chap);
    ~Center();
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *op, QWidget *widget) override;

    int getCenterWidth(){ return cWidth;}
    int getCenterHeight(){ return cHeight;}
    QPoint getCenterPos(){ return centerPos_;}
    int getLv() { return centerLv_;}

    bool chapterFinished();

    // TODO：进入自由演奏状态/判定状态？这个后期优化吧，要交作业肯定是来不及的了
    void noteRecieved(int noteType_);
    void centerPlay(int chap);
    bool playing;
signals:
    void playEnd();

private slots:
    void onNotePlayFinished();

private:
    void addNoteToPlaylist(int noteType);
    void playNextNote();

    QMediaPlayer *centerPlayer;
    QMediaPlayer *notePlayer;
    QAudioOutput *cpOutput;
    QAudioOutput *npOutput;
    QStringList notePlayList;
    QPoint centerPos_;
    enum level{ lv_1 = 1, lv_2 } centerLv_;
    int cWidth;
    int cHeight;
    int chapterNum;
};

#endif // CENTER_H
