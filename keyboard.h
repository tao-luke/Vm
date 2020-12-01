#ifndef ___KEYBOARD___H
#define ___KEYBOARD___H

#include "controller.h"
#include "action.h"
#include <iostream>
#include <map>
#include <set>
class Keyboard : public Controller
{
    std::map<int, Action> keymap;
    Action getActionHelper(int initial);
    Action getActionMovement(int initial,int movement);

public:
    Keyboard();
    Action getAction() override;
    ~Keyboard(){};
    int getActionRaw() override;
};
#endif