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
    void displayFile();
    bool validCursor(int cursorY, int cursorX);
    bool validFirstLine(int firstLine);
    int updateMaxH();
    void scrollDown();
    void displayStatusBar(int c);
    void moveLeftUp(File &file);
    void moveRightDown(File &file);
    void scrollUp();
    void moveCursor(int y, int x);

public:
    NcurseView(Vm & vm); 
    void updateView(Action action) override;
    void updateView(int c) override;
    void displayView() override;

    ~NcurseView() override;
};

#endif
