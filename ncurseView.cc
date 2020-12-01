#include "ncurseView.h"
#include "ncurses.h"
#include "action.h"
#include "vm.h"
#include <memory>
#include <vector>
#include "file.h"
#include <string>
#include <iostream>
NcurseView::NcurseView(Vm &vm):cursorY{0},cursorX{0},screenH{0},screenW{0},maxH{0},firstDisplayLine{0},vm{vm}{
}

void NcurseView::displayFile(){ //handle the display of the content after a firstDisplayLine change
    File file = vm.getFile();
    int x = 0;
    int y = 0;

    //! this part should become fancy later on!!!! //we need to convert each line to special line each type view updates!!!!
    int end = min(maxH, firstDisplayLine + screenH);
    for (int i = firstDisplayLine; i < end ; i++) //! accomate coloring and what not
    { //lines to be shown on screen
            for (auto &c: file.getLineRaw(i)){
                move(y, x++);
                addch(c);
                // printw("and the line# is:");
                // std::string n = std::to_string(i);
                // char const *f = n.c_str();
                // printw(f);
            }
            x = 0;
            y++;
    }
    int tmp = maxH;
    while (tmp < firstDisplayLine + screenH)
    {
        move(y++, 0);
        addch('~');
        tmp++;
    }
}
void NcurseView::displayView(){
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, screenH, screenW);
    screenH--; //make space for status bar
    updateMaxH();

    displayFile(); //display the original file
    move(0, 0);
}
int NcurseView::updateMaxH(){
    maxH = (vm.getFile()).size() ; //save space for status bar
}
bool NcurseView::validCursor(int cursorY, int cursorX){ //return true if simple movement is ok
    //rules for where the cursor can be!
    if (cursorY < 0){
        return false;
    }

    if (cursorY>= screenH){ //case of screen change
        return false;
    }
    if (cursorY+firstDisplayLine >= maxH){
        return false;
    }

    File file = vm.getFile();
    if (cursorX > file.getRawLineSize(cursorY)){
        return false;
    }
    if (cursorX < 0)
        return false;
    return true;
}
bool NcurseView::validFirstLine(int firstLine){ //determine if is it ok to have firsTLine as firstDisplayLine?
    if (firstLine + screenH <= maxH && firstLine >= 0)
        return true;
    return false;
}

void NcurseView::displayStatusBar(int c = -1){
    if (vm.getState() == State::command)
        move(screenH , screenW -75); //! possibly change this magic number to something else!
    else
        move(screenH , screenW - 75);
    
    std::string s = std::to_string(cursorY);
    char const *y = s.c_str();
    std::string b = std::to_string(cursorX);
    char const *x = b.c_str();
    std::string p = std::to_string(maxH);
    char const *H = p.c_str();
    std::string k = std::to_string(screenH+1);
    char const *r = k.c_str();
    std::string n = std::to_string(firstDisplayLine);
    char const *f = n.c_str();
    char const *input;
    if (vm.getState() == State::command)
        printw("command mode");
    else{
        std::string given = std::to_string(c);
        input = given.c_str();
        printw("insert mode");
    }

    printw("   y:");
    printw(y);
    printw(" x:");
    printw(x);
    printw(" maxH: ");
    printw(H);
    printw(" screenH: ");
    printw(r);
    printw(" firstLine:");
    printw(f);
    if (vm.getState() == State::insert)
    {
        printw(" int:");
        printw(input);
    }

}
void NcurseView::updateView(Action action){
    switch(action){
        case Action::up:
             if (cursorY > 0){ //if we can move the cursor at all
                if (validCursor(cursorY-1,cursorX)){ //simple move
                    cursorY--;
                }else if (validCursor(cursorY-1, (vm.getFile()).getRawLineSize(firstDisplayLine+cursorY-1))){ //different line length move
                    cursorY--;
                    cursorX = (vm.getFile()).getRawLineSize(firstDisplayLine+cursorY);
                }
             }else if (validFirstLine(firstDisplayLine-1)){ //at the top already
                firstDisplayLine--;
             }
             break;
        case Action::down:
             if (cursorY+1< screenH){ //if we can move the cursor at all
                if (validCursor(cursorY+1,cursorX)){ //simple move
                    cursorY++;
                }else if (validCursor(cursorY+1, (vm.getFile()).getRawLineSize(firstDisplayLine+cursorY+1))){ //different line length move
                    cursorY++;
                    cursorX = (vm.getFile()).getRawLineSize(firstDisplayLine+cursorY);
                }
             }else if (validFirstLine(firstDisplayLine+1)){ //at the end already
                firstDisplayLine++; 
             }
             break;
        case Action::left:
            if (cursorX > 0)
                cursorX--;
            break;
        case Action::right: 
            if (cursorX < (vm.getFile()).getRawLineSize(cursorY+firstDisplayLine))
                cursorX++;
            break;
        case Action::toCommand:
            vm.setState(State::command);
            break;
        case Action::toInsert:
            vm.setState(State::insert);
            break;
        case Action::deleteLine:
            if (maxH == 1){
                vm.clearLine(cursorY + firstDisplayLine);
            }
            vm.removeLine(cursorY + firstDisplayLine);
            updateMaxH();
            if (!(validCursor(cursorY,cursorX))) //if y is out of bound
                cursorY--;
            break;
        default:
            printw("what is this");
        }
        erase();
        displayFile(); //display the original file
        displayStatusBar();
        move(cursorY, cursorX);
        refresh();
}
void NcurseView::updateView(int c){ //for INsert mode
    if (c != 27)
    {                  //NOT esecape key
        if (c == 127){ //given delete key
            int bef = (vm.getFile()).size();
            int tmp = (vm.getFile()).getRawLineSize(firstDisplayLine+ cursorY-1);
            vm.delCharFromFile(cursorY + firstDisplayLine, cursorX);
            int aft = (vm.getFile()).size();
            if (bef == aft){
                if (cursorX > 0)
                    cursorX--;
            }
            else{
                updateMaxH();
                if (cursorY <= 6 && validFirstLine(firstDisplayLine-1)){ //the 6 is for comfort, so the user can view the parts just before the target cursor
                    firstDisplayLine--; //!confirm is ok
                }else{
                    cursorY--;
                }
                cursorX = tmp;
            }
        }
        else
        {
            int bef = (vm.getFile()).size();
            vm.insertCharToFile(cursorY+firstDisplayLine, cursorX, c);
            int aft = (vm.getFile()).size();
            if (bef == aft)
                cursorX++;
            else{
                updateMaxH();
                if (cursorY+6 >= screenH && validFirstLine(firstDisplayLine+1))
                    firstDisplayLine++;
                else
                    cursorY++;
                cursorX = 0;
            }
        }
    }
    else
    { //escape key
        vm.setState(State::command);
    }
    erase();
    displayFile(); //display the original file
    displayStatusBar(c);
    move(cursorY, cursorX);
    refresh();
}

NcurseView::~NcurseView(){
    endwin();
}