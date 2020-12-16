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
    bool searchMode = false;
    bool opp = false;
    bool colonMode = false;
    Action getActionHelper(int initial, int buffer);
    Action getActionMovement(int initial,int movement);
    std::set<int> awaitKey;
    std::vector<int> queue;
    bool isNumber(int c);
    bool isQueueNumber();
    bool isAwaitCom(int c);
    std::pair<int, Action> handleSearchAction(int c);
    std::pair<int, Action> handleColonAction(int c);
public:
    Keyboard();
    std::pair<int,Action> getAction() override;
    ~Keyboard(){};
    int getActionRaw() override;
};
#endif