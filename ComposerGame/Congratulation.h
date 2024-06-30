#ifndef CONGRATULATION_H
#define CONGRATULATION_H

#define CON_WIDTH 640
#define CON_HEIGHT 420

#include <QDialog>
#include <QToolButton>

namespace Ui {
class Congratulation;
}

class Congratulation : public QDialog
{
    Q_OBJECT

public:
    explicit Congratulation(QWidget *parent = nullptr, int chapter=1);
    ~Congratulation();

protected:
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void updateFinished(int c);

private slots:
    void on_BeltUpBtn__clicked();
    void on_CenterUpBtn__clicked();
    void on_CutterUpBtn__clicked();

private:
    void changeStyleSheet(QToolButton *btn);
    Ui::Congratulation *ui;
    QPainter *conPainter;

    int conChapter_;
};

#endif // CONGRATULATION_H
