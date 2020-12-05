#include "ncurseView.h"
#include "ncurses.h"
#include "action.h"
#include "vm.h"
#include <memory>
#include <vector>
#include "file.h"
#include <string>
#include <iostream>
class badmove{
};
NcurseView::NcurseView(Vm &vm) : cursorY{0}, cursorX{0}, screenH{0}, screenW{0}, maxH{0}, firstDisplayLine{0}, vm{vm}
{
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
            if (file.getLineWithNL(i)){
                move(y-1, screenW - 1);
                addch('1');
            }
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
    if (cursorX > file.getRawLineSize(cursorY+firstDisplayLine)){
        return false;
    }
    if (cursorX < 0)
        return false;
    return true;
}
bool NcurseView::validFirstLine(int firstLine){ //determine if is it ok to have firsTLine as firstDisplayLine?
    if (firstLine+ screenH <= maxH && firstLine >= 0) //we had cursorY instead here //!
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
    std::string k = std::to_string(screenH);
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
void NcurseView::moveLeftUp(File & file){ //move left if possible, else if above is part of this line, move to its end
    if (cursorX > 0)
        cursorX--;
    else
    {
        if (firstDisplayLine+cursorY > 0 && !(file.getLineWithNL(cursorY+firstDisplayLine-1))){
            if (cursorY >= 4 || (firstDisplayLine - 1 < 0))
            { //if we can move the cursor at all //! make this go up at 6 space
                if (validCursor(cursorY - 1, (file).getRawLineSize(firstDisplayLine + cursorY - 1)))
                { //different line length move
                    cursorY--;
                    cursorX = (file).getRawLineSize(firstDisplayLine + cursorY);
                }
            }
            else
            { //at the top already
                firstDisplayLine--;
                if (!(validCursor(cursorY, cursorX)))
                {
                    cursorX = (file).getRawLineSize(firstDisplayLine + cursorY);
                    if (cursorX == screenW)
                        cursorX--;
                }
            }
            if (cursorX == screenW)
                cursorX--;
        }
    }
}
void NcurseView::moveRightDown(File &file){
    if (cursorX + 1 == screenW )
    { //if cursorX is at the end of screen
        if (cursorY+firstDisplayLine+1 < maxH && !(file.getLineWithNL(cursorY+firstDisplayLine))){
            if (cursorY + 4 <= screenH || (firstDisplayLine + 1 + screenH > maxH))
            { //if we can move the cursor at all
                cursorY++;
                cursorX = 0;
            }else
            {
                firstDisplayLine++;
                cursorX = 0;
            }
        }
    }
    else if (validCursor(cursorY, cursorX + 1))
    {
        cursorX++;
    }

}
void NcurseView::updateView(Action action){
    File & file = vm.getFile();
    switch (action)
    {
    case Action::up:
        if (cursorY >= 4 || (firstDisplayLine - 1 < 0))
        { //if we can move the cursor at all //! make this go up at 6 space
            if (validCursor(cursorY - 1, cursorX))
            { //simple move
                cursorY--;
            }
            else if (validCursor(cursorY - 1, (vm.getFile()).getRawLineSize(firstDisplayLine + cursorY - 1)))
            { //different line length move
                cursorY--;
                cursorX = (vm.getFile()).getRawLineSize(firstDisplayLine + cursorY);
            }
        }
        else
        { //at the top already
            firstDisplayLine--;
            if (!(validCursor(cursorY, cursorX)))
            {
                cursorX = (vm.getFile()).getRawLineSize(firstDisplayLine + cursorY);
            }
        }
        break;
    case Action::down:
        if (cursorY + 4 <= screenH || (firstDisplayLine + 1 + screenH > maxH))
        { //if we can move the cursor at all
            if (validCursor(cursorY + 1, cursorX))
            { //simple move
                cursorY++;
            }
            else if (validCursor(cursorY + 1, (vm.getFile()).getRawLineSize(firstDisplayLine + cursorY + 1)))
            { //different line length move
                cursorY++;
                cursorX = (vm.getFile()).getRawLineSize(firstDisplayLine + cursorY);
            }
        }
        else
        { //at the end already
            firstDisplayLine++;
            if (!(validCursor(cursorY, cursorX)))
            {
                cursorX = (vm.getFile()).getRawLineSize(firstDisplayLine + cursorY);
            }
        }
        break;
    case Action::left:
        moveLeftUp(file);
        break;
    case Action::right:
        moveRightDown(file);
        break;
    case Action::toCommand:
        vm.setState(State::command);
        break;
    case Action::toInsert:
        vm.setState(State::insert);
        break;
    case Action::deleteLine:
        if (maxH == 1)
        {
            vm.clearLine(cursorY + firstDisplayLine);
        }
        else
        {
            if (!(file.getLineWithNL(cursorY+firstDisplayLine))){
                int startInc;
                int endInc;
                bool tmp;
                for (int i = cursorY + firstDisplayLine - 1; i >= 0;i--){
                    startInc = i + 1;
                    if (file.getLineWithNL(i))
                        break;
                }
                for (int i = cursorY + firstDisplayLine + 1; i < maxH;i++){
                    endInc = i;
                    if (file.getLineWithNL(i))
                        break;
                }
                for (int i = startInc; i <= endInc;i++){
                    vm.removeLine(i);
                }
                updateMaxH();
                while (!(validCursor(cursorY,cursorX)))
                    cursorY--;
            }
            else
            {
                vm.removeLine(cursorY + firstDisplayLine);
                updateMaxH();
                if (!(validCursor(cursorY, cursorX))) //if y is out of bound
                    cursorY--;
            }
            cursorX = 0;
        }
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
void NcurseView::scrollDown(){
    if (firstDisplayLine+1+ screenH <= maxH ){
        firstDisplayLine++;
    }
}
void NcurseView::updateView(int c){ //for INsert mode 
    File& file = vm.getFile();
    if (c != 27)
    {                  //NOT esecape key
        if (c == 127){ //given delete key
            if (!(cursorX == 0)){
                file.eraseCharFromLine(cursorY + firstDisplayLine, cursorX);
                if (!(file.getLineWithNL(cursorY + firstDisplayLine)))
                {
                    updateMaxH();
                }
                if (cursorY+firstDisplayLine != 0 && file.isLineEmpty(cursorY+firstDisplayLine) && !(file.getLineWithNL(cursorY+firstDisplayLine-1))){
                    file.removeLine(cursorY + firstDisplayLine);
                    file.setLineWithNL(cursorY + firstDisplayLine - 1, false);
                }
                cursorX--;
            }
                /*
                    if line has NL
                        expect simple remove
                    if not
                        expect possible joining from the next line
                        updatemaxh
                    x--;
                    y no change
                */
            else
            {
                if (cursorY+firstDisplayLine != 0){
                    if (atEnd){
                        file.getLineRaw(cursorY+firstDisplayLine-1).pop_back();
                        cursorX = screenW - 1;
                        cursorY--;
                        atEnd = false;
                        erase();
                        displayFile(); //display the original file
                        displayStatusBar(c);
                        move(cursorY, cursorX);
                        refresh();
                        return; //special case
                    }
                    int bef;
                    if (file.getLineWithNL(cursorY + firstDisplayLine - 1) && file.isLineFull(cursorY + firstDisplayLine - 1) && file.isLineEmpty(cursorY + firstDisplayLine))
                    {
                        file.setLineWithNL(cursorY + firstDisplayLine - 1, false);
                        file.removeLine(cursorY + firstDisplayLine);
                        erase();
                        displayFile(); //display the original file
                        displayStatusBar(c);
                        move(cursorY, cursorX);
                        refresh();
                        return;
                        //this is VERY special case :0
                    }
                    if (file.isLineFull(cursorY+firstDisplayLine-1) && !(file.getLineWithNL(firstDisplayLine+cursorY-1)) &&file.isLineEmpty(cursorY+firstDisplayLine)){
                        file.getLineRaw(cursorY+firstDisplayLine-1).pop_back();
                        file.setLineWithNL(cursorY + firstDisplayLine - 1,true);
                        cursorX = screenW - 1;
                        if (cursorY <= 4 && (validFirstLine(firstDisplayLine - 1)))
                        { //the 6 is for comfort, so the user can view the parts just before the target cursor
                            firstDisplayLine--;
                        }else{
                            cursorY--;
                        }
                        erase();
                        displayFile(); //display the original file
                        displayStatusBar(c);
                        move(cursorY, cursorX);
                        refresh();
                        return;
                    } //very specific case
                    bef = file.getRawLineSize(cursorY + firstDisplayLine-1);
                    file.eraseCharFromLine(cursorY + firstDisplayLine, cursorX);
                    updateMaxH();
                    if (file.getLineWithNL(cursorY+firstDisplayLine-1)){
                        cursorX = bef;
                    }
                    else
                    {
                        cursorX = bef - 1;
                        if (cursorX == -1)
                            cursorX++;
                    }
                    if (cursorY <= 4 && (validFirstLine(firstDisplayLine - 1)))
                    { //the 6 is for comfort, so the user can view the parts just before the target cursor
                        firstDisplayLine--;
                    }else{
                        cursorY--;
                    }
                }
                


                /*
                    record the previous line length
                    if line previous has NL
                        expect consumption of previous NL and joining of line and previous
                        updatemaxh
                        x = previous line length before add
                    if not
                        expect joining of line and previous
                        updatemaxh
                        x = previous line length before add -1
                    if bla bla
                        scroll
                    else
                        y--;
                */
            }
        }
        else
        {
            if (c == '\n'){
                if (atEnd){
                    if (firstDisplayLine+cursorY-1 >=0 && file.getLineWithNL(firstDisplayLine+cursorY-1)){
                        file.insertNL(firstDisplayLine + cursorY, true);
                        file.setLineWithNL(firstDisplayLine + cursorY - 1, true);
                    }
                    else if (firstDisplayLine+cursorY >= maxH)
                    {
                        file.insertNL(firstDisplayLine + cursorY, false);
                    }
                    updateMaxH();
                    if (cursorY >= screenH)
                    {
                        scrollDown();
                        cursorY--;
                    }
                    atEnd = false;
                }
                else
                {
                    file.insertCharToLine(cursorY + firstDisplayLine, cursorX, c);
                    updateMaxH();
                    if (cursorY + 1 >= screenH)
                    {
                        scrollDown();
                    }
                    else
                    {
                        cursorY++;
                    }
                    cursorX = 0;
                }
            }else{
                if (atEnd){
                    if (firstDisplayLine+cursorY-1 >=0 && file.getLineWithNL(firstDisplayLine+cursorY-1)){
                        file.insertNL(firstDisplayLine + cursorY, true);
                        file.setLineWithNL(firstDisplayLine + cursorY - 1, false);
                    }
                    else if (firstDisplayLine+cursorY >= maxH)
                    {
                        file.insertNL(firstDisplayLine + cursorY, false);
                    }

                    file.insertCharToLine(firstDisplayLine + cursorY, cursorX, c);
                    updateMaxH();
                    if (cursorY >= screenH)
                    {
                        scrollDown();
                        cursorY--;
                    }
                    cursorX++;
                    atEnd = false;
                }else{
                    int before = maxH;
                    file.insertCharToLine(firstDisplayLine + cursorY, cursorX, c);
                    updateMaxH();
                    if (cursorX + 1 < screenW)
                    {
                        cursorX++;
                    }
                    else
                    {
                        cursorY++;
                        cursorX = 0;
                        atEnd = true;
                    }
                }
            }
        }
    }
    else{ //escape key
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