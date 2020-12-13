#ifndef ___NCURSEVIEW___H
#define ___NCURSEVIEW___H

#include "view.h"
#include "action.h"
#include "vm.h"
#include <vector>


class NcurseView : public View
{

    //make sure we take the actual concret vim class here.
    int cursorY, cursorX, screenH, screenW, maxH, firstDisplayLine;
    Vm &vm;
    std::vector<int> queue;
    bool focusedMode = false;
    bool showFileInfo = true;
    void displayFile();
    bool validCursor(int cursorY, int cursorX);
    bool validFirstLine(int firstLine);
    int updateMaxH();
    void scrollDown();
    void displayStatusBar();
    std::pair<int,int> moveLeftUp();
    std::pair<int,int> moveRightDown();
    std::pair<int, int> notStrictMoveRightDown();
    std::pair<int, int> moveUp();
    std::pair<int,int> moveDown();
    bool validateY(int y);
    bool validateX(int y,int x);
    void scrollUp();
    void moveCursor(int y, int x);
    void removeCharHelper(int line, int x,bool copy,Action action);
    Action init(Action action);
    std::pair<int,int> leftUpSearch();
    std::pair<int,int> rightDownSearch();
public:

    void refreshView() override;
    NcurseView(Vm & vm); 
    void updateView(std::pair<int,Action> input) override;
    void updateView(int c) override;
    void displayView() override;

    ~NcurseView() override;
};

#endif
