#include "GlobalState.h"


GlobalState::GlobalState()
{
    ;// coins = 10000; 这段代码可以修改静态变量的值
}

int GlobalState::coins = 500;
int GlobalState::centerLevel = 1;
int GlobalState::noteBlockLevel = 1;
int GlobalState::noteValue = 1;
