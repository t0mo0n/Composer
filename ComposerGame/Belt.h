#ifndef BELT_H
#define BELT_H

#include <QGraphicsObject>
#include "Components.h"

class Belt : public Components
{
    Q_OBJECT
public:
    Belt();
private:
    enum direction {LeftToRight = 1, UpToDown,RightToLeft,DownToUp,DownToRight,DownToLeft,
                     LeftToDown,LeftToUp,UpToLeft,UpToRight,RightToUp,RightToDown} beltDirection_;
    QPoint beltPos_;

};

#endif // BELT_H
