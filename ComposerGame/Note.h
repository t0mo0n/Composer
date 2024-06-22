#ifndef NOTE_H
#define NOTE_H

#include <QGraphicsItem>

class Note : public QGraphicsItem
{
public:
    Note();
    void noteMove();
private:
    QPoint notePos_;

};

#endif // NOTE_H
