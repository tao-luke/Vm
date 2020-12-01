#ifndef ___CONTROLLER___H
#define ___CONTROLLER___H

#include "action.h"

class Controller{
    public:
        virtual Action getAction() = 0;
        virtual ~Controller(){};
        virtual int getActionRaw() = 0;
};
#endif