#ifndef ___EVENT___H
#define ___EVENT___H

#include "action.h"
#include <vector>
#include <memory>
#include "vm.h"
class Event{
    int cursorY;
    int firstDisplayLine;
    int cursorX;
    Action action;
    std::vector<std::vector<char>> bufferOld; //optional.
    std::vector<char> content;
    Vm &vm;

public:
    Event(int cursorY, int firstDisplayLine, int cursorX, Action action, std::vector<char> content, Vm & vm) : cursorY{cursorY}, firstDisplayLine{firstDisplayLine}, cursorX{cursorX}, action{action},content{content},vm{vm}{};
    void setBufferOld(std::vector<std::vector<char>> snapshot){
        bufferOld = snapshot;
    }
    void undo(){
        switch(action){
            case joinThisAndNextWithSpace:
                break;
            case deleteLine:
                break;
            case deleteLeft:
                break;
            case deleteRight:
                break;
            case deleteUp:
                break;
            case deleteDown:
                break;
            case deleteLeftNC:
                break;
            case deleteRightNC:
                break;
            case deleteUpNC:
                break;
            case deleteDownNC:
                break;
            case clearLine:
                break;
            case deleteChar:
                break;
            case deleteCharThenInsert:
                break;
            case replaceCharWith:
                break;
            case pasteAfterCursor:
                break;
            case pasteBeforeCursor:
                break;
            case insertNLAboveAndInsert:
                break;
            case insertNLUnderAndInsert:
                break;
            
            }
    }
};

#endif
