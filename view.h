#ifndef ___VIEW___H
#define ___VIEW___H

#include "action.h"
#include <utility>
class View{
    public:
    virtual void updateView(std::pair<int,Action> data) = 0;
    virtual void updateView(int c) = 0;
    virtual void refreshView() = 0;
    virtual void displayView() = 0;
    virtual ~View(){};
};
#endif

