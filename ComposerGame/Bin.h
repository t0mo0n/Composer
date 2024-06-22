#ifndef BIN_H
#define BIN_H

#include <QGraphicsObject>
#include "Components.h"

class Bin : public Components
{
public:
    Bin();
private:
    QPoint binPos_;

};

#endif // BIN_H
