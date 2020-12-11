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
    if ( cursorX >= file.getRawLineSize(cursorY+firstDisplayLine)){
        if (!(cursorX ==0 && file.getRawLineSize(cursorY+firstDisplayLine) == 0)){
            return false;
        }

    }
    if (cursorX < 0)
        return false;
    return true;
}
bool NcurseView::validateY(int cursorY){
    if (cursorY < 0){
        return false;
    }

    if (cursorY>= screenH){ //case of screen change
        return false;
    }
    if (cursorY+firstDisplayLine >= maxH){
        return false;
    }
    return true;
}
bool NcurseView::validateX(int y,int cursorX){
    File file = vm.getFile();
    if ( cursorX >= file.getRawLineSize(y+firstDisplayLine)){
        if (!(cursorX ==0) || !(file.getRawLineSize(y+firstDisplayLine) == 0)){
            return false;
        }

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
        printw(" input:");
        printw(input);
    }else{
        printw(" queue:");
        for(auto &c:queue){
            addch(c);
        }
    }
}
std::pair<int,int> NcurseView::moveLeftUp(){ //move left if possible, else if above is part of this line, move to its end
    int x = cursorX;
    int y = cursorY;
    File file = vm.getFile(); //! rewrite these 2 functions
    if (x > 0)
        x--;
    else
    {
        int line = firstDisplayLine + y;
        if (file.getBeginIndexOnLine(line) != 0){
            y--;
            x = file.getRawLineSize(line - 1);
            if (x != 0){
                x--;
            }
        }
    }
    return std::pair<int, int>(y, x);
}
std::pair<int,int> NcurseView::moveRightDown(){
    int x = cursorX;
    int y = cursorY;
    File file = vm.getFile();
    int line = firstDisplayLine+y;
    int max = min(screenW, file.getRawLineSize(line));
    if (x + 1 >= max)
    { //if x is at the end of screen
        if (!(file.getLineWithNL(line))){
            y++;
            x = 0;
        }
    }
    else
    {
        x++;
    }
    return std::pair<int, int>(y, x);
}

std::pair<int,int> NcurseView::notStrictMoveRightDown(){
    int x = cursorX;
    int y = cursorY;
    File file = vm.getFile();
    int line = firstDisplayLine+y;
    int max = min(screenW, file.getRawLineSize(line)+1);
    if (x + 1 >= max)
    { //if x is at the end of screen
        if (!(file.getLineWithNL(line))){
            y++;
            x = 0;
        }else{
            x++;
        }
    }
    else
    {
        x++;
    }
    return std::pair<int, int>(y, x);
}
std::pair<int,int> NcurseView::moveUp(){
    int x = cursorX;
    int y = cursorY;
    if (validateY(y-1)){
        y--;
        while (validateY(y-1) &&vm.getFile().getBeginIndexOnLine(firstDisplayLine + y) != 0)
        {
            y--;
        }

        if (!(validateX(y,x))){
            std::pair<int,int> data = vm.endLineCoord(y+firstDisplayLine); 
            if (data.first != -1){
                x = data.second;
                if (x != 0){
                    x--;
                }
                y = data.first - firstDisplayLine;
            }
        }
    }
    return std::pair<int, int>(y, x);
}

std::pair<int,int> NcurseView::moveDown(){
    int x = cursorX;
    int y = cursorY;
    if (validateY(y+1)){
        y++;
        while (validateY(y+1) &&vm.getFile().getBeginIndexOnLine(firstDisplayLine + y) != 0)
        {
            y++;
        }
        if (!(validateX(y,x))){
            std::pair<int,int> data = vm.endLineCoord(y+firstDisplayLine); 
            if (data.first != -1){
                x = data.second;
                if (x != 0){
                    x--;
                }
                y = data.first - firstDisplayLine;
            }
        }
    }
    return std::pair<int, int>(y, x);
}
void NcurseView::removeCharHelper(int line, int x,bool copy,Action action){
    bool bef;
    if (line != 0)
    {
        bef = vm.getFile().getLineWithNL(line - 1);
    }

    
    if (!(vm.getFile().getRawLineSize(line) == 0 && vm.getFile().getBeginIndexOnLine(line) == 0)) //if this line is NOT empty and 
    {
        vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,action);
        vm.deleteCharSimple(line, x,copy);
        updateMaxH();

        if (x >= vm.getFile().getRawLineSize(line))
        {
            if (x != 0){
                cursorX--;
            }

            
        }
        if (line != 0){
            if (bef != vm.getFile().getLineWithNL(line - 1)){
                cursorY--;
                cursorX = vm.getFile().getRawLineSize(line - 1) - 1;
            }
        }
    }
}
void NcurseView::updateView(std::pair<int,Action> input){
    int c = input.first; //current command char
    if (input.second == Action::await){
        queue.push_back(c);
    }else{
        if (!(queue.empty())){
            queue.clear();
        }
    }
    File &file = vm.getFile();           //easy file access
    int line = cursorY+firstDisplayLine; //line on
    int tmp; //to copy any int
    std::pair<int, int> data;
    std::vector<int> info;
    switch (input.second)
    {
    case Action::invalid:
        break;
    case Action::await:
        break;
    case Action::up:
        data = moveUp();
        cursorX = data.second;
        cursorY = data.first; 
        scrollUp();
        break;
    case Action::down:
        data = moveDown();
        cursorX = data.second;
        cursorY = data.first;
        scrollDown();
        break;
    case Action::left:
        data = moveLeftUp();
        cursorX = data.second;
        cursorY = data.first;
        scrollUp();
        break;
    case Action::right:
        data = moveRightDown();
        cursorX = data.second;
        cursorY = data.first;
        scrollDown();
        break;
    case Action::undo:
        if (!(vm.isUndoStackEmpty())){
            info = std::move(vm.handleUndo());
            cursorY = info[0];
            firstDisplayLine = info[1];
            cursorX = info[2];
            maxH = info[3];
        }
        break;
    case Action::clearLine:
        if (validateY(cursorY) && file.getRawLineSize(line) != 0){
            vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
            tmp = line;
            while(file.getBeginIndexOnLine(tmp--) != 0){
                cursorY--;
            }
            vm.clearLineWithFormat(line);
            updateMaxH();
            cursorX = 0;
        }
        vm.setState(State::insert);
        break;
    case Action::deleteLeftNC:
        data = moveLeftUp();
        if (!(cursorX == data.second && cursorY == data.first)){
            vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
            cursorX = data.second;
            cursorY = data.first;
            vm.deleteCharSimple(line, cursorX,false);
        }
        vm.setState(State::insert);
        break;
    case Action::deleteRightNC:
        removeCharHelper(line, cursorX,false,input.second);
        vm.setState(State::insert);
        break;
    case Action::deleteUpNC:
        data = moveUp();
        if (!(cursorX == data.second && cursorY == data.first)){
            vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
            vm.removeLineFromFile(data.first + firstDisplayLine, false);
            vm.setSkipRecord(true);
            vm.removeLineFromFile(data.first + firstDisplayLine, false);
            updateMaxH();
            
            cursorX = data.second;
            cursorY = data.first;
            while (!(validateY(cursorY))){
                cursorY--;
            }
        }
        scrollUp();
        vm.setState(State::insert);
        break;
    case Action::deleteDownNC:
        data = moveDown();
        if (!(cursorX == data.second && cursorY == data.first)){
            vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
            vm.removeLineFromFile(data.first + firstDisplayLine, false);
            vm.setSkipRecord(true);
            vm.removeLineFromFile(line,false);
            updateMaxH();
            cursorX = data.second;
            while (!(validateY(cursorY))){
                cursorY--;
            }
        }
        scrollDown();
        vm.setState(State::insert);
        break;
    case Action::deleteLeft:
        data = moveLeftUp();
        if (!(cursorX == data.second && cursorY == data.first)){
            vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second); //if it runs, we should prepare for undo
            cursorX = data.second;
            cursorY = data.first;
            vm.deleteCharSimple(line, cursorX,true);
        }
        break;
    case Action::deleteRight:
        removeCharHelper(line, cursorX,true,input.second);
        break;
    case Action::deleteUp:
        data = moveUp();
        if (!(cursorX == data.second && cursorY == data.first)){
            vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
            vm.removeLineFromFile(data.first + firstDisplayLine, false);
            vm.setSkipRecord(true);
            vm.removeLineFromFile(data.first + firstDisplayLine, true);
            updateMaxH();
            
            cursorX = data.second;
            cursorY = data.first;
            while (!(validateY(cursorY))){
                cursorY--;
            }
        }
        scrollUp();
        break;
    case Action::deleteDown:
        data = moveDown();
        if (!(cursorX == data.second && cursorY == data.first)){
            vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
            vm.removeLineFromFile(data.first + firstDisplayLine, false);
            vm.setSkipRecord(true);
            vm.removeLineFromFile(line,true);
            updateMaxH();
            cursorX = data.second;
            while (!(validateY(cursorY)))
            {
                cursorY--;
            }
        }
        scrollDown();
        break;
    case Action::copyCurrentLine:
        vm.copyLineFromFile(line,false);
        break;
    case Action::copyLeft:
        data = moveLeftUp();
        if (!(cursorX == data.second && cursorY == data.first)){
            cursorX = data.second;
            cursorY = data.first;
            vm.copyChar(line, cursorX);
        }
        break;
    case Action::copyRight:
        vm.copyChar(line,cursorX);
        break;
    case Action::copyDown:
        data = moveDown();
        if (!(cursorX == data.second && cursorY == data.first)){
            vm.copyLineFromFile(line,false);
            vm.copyLineFromFile(line + 1,true);
        }
        break;
    case Action::copyUp:
        data = moveUp();
        if (!(cursorX == data.second && cursorY == data.first)){
            vm.copyLineFromFile(line-1,false);
            vm.copyLineFromFile(line ,true);
            updateMaxH();
            cursorX = data.second;
            cursorY = data.first;
        }
        scrollUp();
        break;
    case Action::toLastChar:
        data = vm.endLineCoord(line); 
        if (data.first != -1){
            cursorX = data.second;
            if (cursorX != 0){
                cursorX--;
            }
            cursorY = data.first - firstDisplayLine;
        }
        scrollDown();
        break;
    case Action::moveCursorToLineEndThenInsert:
        data = vm.endLineCoord(line); 
        if (data.first != -1){
            cursorX = data.second;
            cursorY = data.first - firstDisplayLine;
        }
        scrollDown();
        vm.setState(State::insert);
        break;
    case Action::moveToNextChar:
        if (!(file.getRawLineSize(line) ==0) && cursorX < file.getRawLineSize(line) ){
            //if this line isnt empty and cursorX is valid
            data = vm.nextCharCoord(line, cursorX, c);
            if (data.first != -1){
                cursorX = data.second;
                cursorY = data.first - firstDisplayLine;
            }
            scrollDown();
        }
        break;
    case Action::moveToPreviousChar:
        if (!(file.getRawLineSize(line) ==0) && cursorX > 0 ){
            //if this line isnt empty and cursorX is valid
            data = vm.previousCharCoord(line, cursorX, c);
            if (data.first != -1){
                cursorX = data.second;
                cursorY = data.first - firstDisplayLine;
            }
            scrollUp();
        }
        break;
    case Action::toFirstChar:
        while (file.getBeginIndexOnLine(line) != 0 && line >0){
            line--;
        }
        cursorX = 0;
        cursorY = line - firstDisplayLine;
        scrollUp();
        break;
    case Action::moveCursorToFrontThenInsert:
        while (file.getBeginIndexOnLine(line) != 0 && line >0){
            line--;
        }
        cursorX = 0;
        cursorY = line - firstDisplayLine;
        scrollUp();
        vm.setState(State::insert);
        break;
    case Action::toCommand:
        vm.setState(State::command);
        break;
    case Action::toInsert:
        vm.setState(State::insert);
        break;
    case Action::joinThisAndNextWithSpace:
        vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
        data = vm.joinThisAndNextWithSpace(line, cursorX);
        if (data.first != -1){
            cursorX = data.second;
            cursorY = data.first - firstDisplayLine;
        }
        updateMaxH();
        break;
    case Action::toInsertNext:
        vm.setState(State::insert);
        if (!(file.getRawLineSize(line) == 0)){
            data = notStrictMoveRightDown();
            cursorX = data.second;
            cursorY = data.first;
        }
        break;
    case Action::toPreviousWord:
        data = vm.previousWordCoord(line, cursorX);
        if (cursorY == data.first-firstDisplayLine && cursorX == data.second && line > 0){
            data = vm.previousWordCoord(line-1, vm.getFile().getRawLineSize(line - 1));
        }
        cursorY = data.first - firstDisplayLine;
        cursorX = data.second;
        scrollUp();
        break;
    case Action::toNextWord:
        data = vm.nextWordCoord(line, cursorX);
        if (cursorY == data.first-firstDisplayLine && cursorX == data.second && line+1 < file.lineTotal()){
            data = vm.nextWordCoord(line+1, -1);
        }
        if (data.second != -1){
            cursorX = data.second;
            cursorY = data.first - firstDisplayLine;
            scrollDown();
        }

        break;
    case Action::toFirstNonBlank:
        data = vm.firstNonBlankCoord(line, cursorX);
        cursorX = data.second;
        cursorY = data.first - firstDisplayLine;
        break;
    case Action::deleteChar:
        removeCharHelper(line, cursorX,true,input.second);
        break;
    case Action::deleteCharThenInsert:
        removeCharHelper(line, cursorX,false,input.second);
        vm.setState(State::insert);
        break;
    case Action::replaceCharWith:
        if (cursorX < file.getRawLineSize(line)){ //cursorX must be in bound
            vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
            vm.replaceCharAt(line, cursorX, c);
        }
        break;
    case Action::insertNLUnderAndInsert:
        if (line< maxH){
            tmp = line+1;
            cursorY++;
            while (tmp < maxH && file.getBeginIndexOnLine(tmp++) != 0)
            {
                cursorY++;
            }
        }
        vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
        vm.insertNLBehind(line, cursorX);
        cursorX = 0;
        updateMaxH();

        scrollDown();

        vm.setState(State::insert);
        break;
    case Action::insertNLAboveAndInsert:
        tmp = line;
        while (file.getBeginIndexOnLine(tmp) != 0)
        {
            tmp--;
        }
        vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
        vm.insertNLAbove(line, cursorX);
        cursorX = 0;
        cursorY = tmp - firstDisplayLine;
        updateMaxH();
        scrollUp();
        vm.setState(State::insert);
        break;
    case Action::deleteLine:
        vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
        vm.removeLineFromFile(line,false);
        updateMaxH();
        if (cursorY+firstDisplayLine >= maxH){
            cursorY = maxH - firstDisplayLine - 1;
            scrollUp();
        }
        cursorX = 0;
        break;
    case Action::pasteAfterCursor:
        data = vm.pasteAfterCursor(cursorY,firstDisplayLine,maxH,cursorX);
        if (data.first != -1){
            updateMaxH();
            cursorX = data.second;
            cursorY = data.first - firstDisplayLine;
            if (cursorX > screenW){
                cursorX = 1;
                cursorY++;
            }
            scrollDown();
        }
        break;
    case Action::pasteBeforeCursor:
        data = vm.pasteBeforeCursor(cursorY,firstDisplayLine,maxH,cursorX);
        if (data.first != -1){
            updateMaxH();
            cursorX = data.second;
            cursorY = data.first - firstDisplayLine;
        }
        break;
    default:
        break;
    }
    moveCursor(cursorY, cursorX);
}
void NcurseView::refreshView(){
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
    while (cursorY >= screenH-4){
        if (firstDisplayLine+1+ screenH > maxH ){
            return;
        }
        firstDisplayLine++;
        cursorY--;
    }
}

void NcurseView::scrollUp(){
    while (cursorY <= 4){
        if (firstDisplayLine-1 < 0){
            return;
        }
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
            scrollUp();
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
            scrollDown();
        }
    }
    else{ //escape key
        if (validCursor(cursorY,cursorX-1)){
            cursorX--;
        }
        vm.setState(State::command);
        displayStatusBar(c);
        moveCursor(cursorY, cursorX);
        refresh();
        return;
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