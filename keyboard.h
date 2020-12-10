#ifndef ___KEYBOARD___H
#define ___KEYBOARD___H

#include "controller.h"
#include "action.h"
#include <iostream>
#include <map>
#include <vector>
#include <set>

class Keyboard : public Controller
{
    std::map<int, Action> keymap;
    Action getActionHelper(int initial,int buffer);
    Action getActionMovement(int initial,int movement);
    std::set<int> awaitKey;
    std::vector<int> queue;

public:
    Keyboard();
    std::pair<int,Action> getAction() override;
    ~Keyboard(){};
    int getActionRaw() override;
};
#endif