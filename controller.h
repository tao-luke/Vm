#ifndef ___CONTROLLER___H
#define ___CONTROLLER___H

#include "action.h"
#include <utility>
class Controller
{
public:
    virtual std::pair<int, Action> getAction() = 0;
    virtual ~Controller(){};
    virtual int getActionRaw() = 0;
};
#endif