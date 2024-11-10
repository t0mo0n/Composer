#ifndef MUSICST_H
#define MUSICST_H

#define MST_WIDTH 864
#define MST_HEIGHT 625
#include <QToolButton>
#include <QDialog>
#include <QMediaPlayer>
#include <QAudioOutput>

namespace Ui {
class MusicST;
}

class MusicST : public QDialog
{
    Q_OBJECT

public:
    explicit MusicST(QWidget *parent = nullptr);
    ~MusicST();

protected:
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void changeStyleSheet(QToolButton *btn);

private:
    bool playing_;

    Ui::MusicST *ui;
    QPainter *MST_painter;
    QMediaPlayer *MST_player;
    QAudioOutput *MST_Output;

signals:
    void mstWindowClosed();
private slots:
    void on_tB_1_clicked();
    void on_tB_2_clicked();
    void on_tB_3_clicked();
    void on_tB_4_clicked();
    void on_tB_5_clicked();
    void on_tB_6_clicked();
    void on_tB_7_clicked();
};

#endif // MUSICST_H
