#ifndef ___EVENT___H
#define ___EVENT___H

#include "action.h"
#include <vector>
#include <memory>
class Vm;
class Event
{
    int cursorY;
    int firstDisplayLine;
    int cursorX;
    int maxH;
    Action action;
    std::vector<std::vector<char>> content;
    Vm &vm;
    std::pair<int, int> data;
    int pasteLN;

public:
    Event(int cursorY, int firstDisplayLine, int cursorX,int maxH, Action action, Vm & vm) : cursorY{cursorY}, firstDisplayLine{firstDisplayLine}, cursorX{cursorX},maxH{maxH}, action{action},vm{vm}{};
    void setContent(std::vector<std::vector<char>> content, std::pair<int, int> data, int pasteLN = 0);
    std::vector<int> undo();
    void contentPushback(std::vector<char> pushedback);
};

#endif

