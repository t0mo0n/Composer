#ifndef ENTRANCEWINDOW_H
#define ENTRANCEWINDOW_H

#define EW_WIDTH 1318
#define EW_HEIGHT 892

#include <QWidget>
#include <QLabel>
#include <QToolButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class EntranceWindow;
}
QT_END_NAMESPACE

class EntranceWindow : public QWidget
{
    Q_OBJECT

public:
    EntranceWindow(QWidget *parent = nullptr);
    void changeStyleSheet(QToolButton *btn);
    ~EntranceWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void displayText();

private slots:
    void on_ShopButton__clicked();

private:
    Ui::EntranceWindow *ui;

    QPainter* entrancePainter;
};
#endif // ENTRANCEWINDOW_H
