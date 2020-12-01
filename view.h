#ifndef ___VIEW___H
#define ___VIEW___H

#include "action.h"
class View{
    public:
    virtual void updateView(Action action) = 0;
    virtual void updateView(int c) = 0;
    virtual void displayView() = 0;
    virtual ~View(){};
};
#endif