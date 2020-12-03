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
    bool atEnd = false;
    Vm &vm;
    void displayFile();
    bool validCursor(int cursorY, int cursorX);
    bool validFirstLine(int firstLine);
    int updateMaxH();
    void moveRight();
    void scrollDown();
    void displayStatusBar(int c);

public:
    NcurseView(Vm & vm); 
    void updateView(Action action) override;
    void updateView(int c) override;
    void displayView() override;

    ~NcurseView() override;
};

#endif
