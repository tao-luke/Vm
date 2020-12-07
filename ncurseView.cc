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
            move(y-1, screenW+1);
            if (file.getLineWithNL(i)){
                addch('1');
            }else{
                addch('0');
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
    screenW--;
    screenW--;
    screenW--;
    updateMaxH();
    displayFile(); //display the original file
    move(0, 0);
}
int NcurseView::updateMaxH(){
    maxH = (vm.getFile()).lineTotal() ; //save space for status bar
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
        move(screenH , screenW -70); //! possibly change this magic number to something else!
    else
        move(screenH , screenW - 70);
    
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
void NcurseView::removeCharSimple(File& file,int line, int x){
    bool bef;
    if (line != 0)
    {
        bef = vm.getFile().getLineWithNL(line - 1);
    }

    if (!(file.getRawLineSize(line) == 0 && file.getBeginIndexOnLine(line) == 0))
    {
        vm.deleteCharSimple(line, x);
        updateMaxH();
        if (x > file.getRawLineSize(line))
        {
            cursorX--;
        }
        if (line != 0){
            if (bef != vm.getFile().getLineWithNL(line - 1)){
                cursorY--;
                cursorX = vm.getFile().getRawLineSize(line - 1) - 1;
            }
        }
    }
}
void NcurseView::updateView(Action action){
    File & file = vm.getFile();
    int line = cursorY+firstDisplayLine;
    int tmp;
    std::pair<int, int> data;
    switch (action)
    {
    case Action::up:
        if (cursorY >= 4 || (firstDisplayLine - 1 < 0))
        { //if we can move the cursor at all //! make this go up at 6 space
            if (validCursor(cursorY - 1, cursorX))
            { //simple move
                cursorY--;
            }
            else if (validCursor(cursorY - 1, (vm.getFile()).getRawLineSize(line - 1)))
            { //different line length move
                cursorY--;
                cursorX = (vm.getFile()).getRawLineSize(line);
            }
        }
        else
        { //at the top already
            firstDisplayLine--;
            if (!(validCursor(cursorY, cursorX)))
            {
                cursorX = (vm.getFile()).getRawLineSize(line);
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
            else if (validCursor(cursorY + 1, (vm.getFile()).getRawLineSize(line + 1)))
            { //different line length move
                cursorY++;
                cursorX = (vm.getFile()).getRawLineSize(line);
            }
        }
        else
        { //at the end already
            firstDisplayLine++;
            if (!(validCursor(cursorY, cursorX)))
            {
                cursorX = (vm.getFile()).getRawLineSize(line);
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
    case Action::toInsertNext:
        vm.setState(State::insertNext);
        if (validCursor(cursorY,cursorX+1)){
            cursorX++;
        }
        break;
    case Action::toPreviousWord:
        data = vm.previousWordCoord(line, cursorX);
        if (cursorY == data.first-firstDisplayLine && cursorX == data.second && line > 0){
            data = vm.previousWordCoord(line-1, vm.getFile().getRawLineSize(line - 1));
        }
        cursorY = data.first - firstDisplayLine;
        cursorX = data.second;
        if (cursorY <= 4){
            scrollUp();
        }
        break;
    case Action::deleteChar:
        removeCharSimple(file,line, cursorX);
        vm.setState(State::insert);
        break;
    case Action::deleteCharThenInsert:
        removeCharSimple(file,line, cursorX);

        break;
    case Action::clearLine:
        tmp = line;
        while(file.getBeginIndexOnLine(tmp--) != 0){
            cursorY--;
        }
        vm.clearLineWithFormat(line);
        updateMaxH();
        cursorX = 0;
        vm.setState(State::insert);
        break;
    case Action::deleteLine:
        if (maxH == 1)
        {
            vm.clearLine(line);
        }
        else
        {
            vm.removeLineFromFile(line);
            updateMaxH();
            if (cursorY+firstDisplayLine >= maxH){
                cursorY = maxH - firstDisplayLine - 1;
                if (cursorY <= 4){
                    scrollUp();
                }
            }
            cursorX = 0;
        }
        break;
    default:
        printw("what is this");
        break;
    }
        erase();
        displayFile(); //display the original file
        displayStatusBar();
        moveCursor(cursorY, cursorX);
        refresh();
}
void NcurseView::moveCursor(int y, int x){
    if (x == screenW){
        y++;
        x = 0;
    }
    move(y, x);
}
void NcurseView::scrollDown(){
    if (firstDisplayLine+1+ screenH <= maxH ){
        firstDisplayLine++;
        cursorY--;
    }
}

void NcurseView::scrollUp(){
    if (firstDisplayLine-1 >=0){
        firstDisplayLine--;
        cursorY++;
    }
}

void NcurseView::updateView(int c){ //for INsert mode 
    if (c != 27)
    {                  //NOT esecape key
    int line = cursorY + firstDisplayLine;
        if (c == 127){ //given delete key
            int beginIndex = vm.getFile().getBeginIndexOnLine(line); //! still need fixing, cant handle at end then not at end
            if (cursorX == 0) //crusorx = 0
            {
                if (!(line == 0)){ //not at x,y == 0,0
                    int tmp = vm.getFile().getRawLineSize(line-1);
                    vm.delCharFromFile(line, cursorX);
                    cursorY--;
                    if (beginIndex == 0){ //prepare for merge
                        cursorX = tmp;
                    }
                    else
                    {
                        cursorX = tmp - 1;
                    }
                }
            }
            else
            {
                vm.delCharFromFile(line, cursorX);
                if (beginIndex != 0 && cursorX==1){
                    cursorY--;
                    cursorX = screenW;
                }
                else
                {
                    cursorX--;
                }
            }
            updateMaxH();
            if (cursorY <= 4)
            {
                scrollUp();
            }
        }
        else
        {
            vm.insertCharToFile(line, cursorX++, c);
            updateMaxH();           
            if (c == '\n'){
                cursorX = 0;
                cursorY++;
            }
            else
            { //if anything but space, we just insert and adjust cursor accordingly
                if (cursorX > screenW)
                {
                    cursorX = 1;
                    cursorY++;
                }
            }
            if (cursorY >= screenH-4){
                scrollDown();
            }
        }
    }
    else{ //escape key
        if (validCursor(cursorY,cursorX-1)){
            cursorX--;
        }
        vm.setState(State::command);
    }
    erase();
    displayFile(); //display the original file
    displayStatusBar(c);
    moveCursor(cursorY, cursorX);
    refresh();
}

NcurseView::~NcurseView(){
    endwin();
}