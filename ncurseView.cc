#include "ncurseView.h"
#include "ncurses.h"
#include "action.h"
#include "vm.h"
#include <memory>
#include <vector>
#include "file.h"
#include <string>
#include <iostream>
#include <stdlib.h>
class badmove{
};
NcurseView::NcurseView(Vm &vm) : cursorY{0}, cursorX{0}, screenH{0}, screenW{0}, maxH{0}, firstDisplayLine{0}, vm{vm}
{
}

void NcurseView::displayFile(){ //display the screen with a cursorpos and firstdisplayline coord
    File file = vm.getFile();
    int x = 0;
    int y = 0;
   
    int end = min(maxH, firstDisplayLine + screenH);
    for (int i = firstDisplayLine; i < end ; i++) 
    { //lines to be shown on screen
        file.getLine(i).displayLine(y, x);
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
void NcurseView::displayView(){ //initialize the ncurseview for display
    noecho();
    keypad(stdscr, TRUE);
    if (vm.highlightOption()){
        start_color();
    }
    
    getmaxyx(stdscr, screenH, screenW);
    screenH--; //make space for status bar
    updateMaxH();
    displayFile(); //display the original file
    move(0, 0);
}
int NcurseView::updateMaxH(){ //update the maxH when file contents have been modified
    maxH = (vm.getFile()).lineTotal(); 
}
bool NcurseView::validCursor(int cursorY, int cursorX){ //return true if the given cursor pos is valid
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
bool NcurseView::validateY(int cursorY){ //return true if valid y pos given
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
bool NcurseView::validateX(int y,int cursorX){ //return true if valid x pos given
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
    if (firstLine+ screenH <= maxH && firstLine >= 0) //we had cursorY instead here 
        return true;
    return false;
}

void NcurseView::displayStatusBar(){ //display the statusbar
    move(screenH, 0);
    if (focusedMode){
        for(auto & c: queue){
            addch(c);
        }
    }else{
        if (vm.getState() == State::insert){ //mode
            printw("-- INSERT -- ");
        }else{
            if (showFileInfo){
                printw("\"");
                printw(vm.getName());
                printw("\"");
                move(screenH, 12);
                if (vm.getModified()){
                    printw("[Mod]");
                }
                std::string num = std::to_string(maxH);
                char const *totalline = num.c_str();
                printw(totalline);
                printw(" lines");
                std::string currentPHelper = " --" + std::to_string((((cursorY + firstDisplayLine) * 100 / maxH)));
                char const *currentPer = currentPHelper.c_str();
                printw(currentPer);
                addch('%');
                printw("-- ");
                std::string stackleft = std::to_string(vm.getStackSize());
                char const *totalstack = stackleft.c_str();
                printw(totalstack);
                printw(" undo(s)in stack");
            }
        }

        move(screenH, screenW - 15);
        for(auto &c:queue){
            addch(c);
        }


        move(screenH, screenW - 10); //cursorInfo
        std::pair<int, int> data = vm.convertCursor(cursorY + firstDisplayLine, cursorX);
        std::string lineInfo = std::to_string(cursorY) + "," + std::to_string(cursorX); //!
        char const *cursorPos = lineInfo.c_str();
        printw(cursorPos);


        move(screenH, screenW-3 ); //percent sign
        int tmp = ((screenH + firstDisplayLine)*100 / maxH);
        if (firstDisplayLine >= maxH-screenH){
            printw("Bot");
        }else if (firstDisplayLine == 0){
            printw("Top");
        }else{
            std::string tmp1 = std::to_string(tmp);
            char const *percent = tmp1.c_str();
            printw(percent);
            addch('%');
        }
    }
    
}
std::pair<int,int> NcurseView::moveLeftUp(){ //move left if possible, else if above is part of this line, move to its end
    int x = cursorX;
    int y = cursorY;
    File file = vm.getFile(); 
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
std::pair<int,int> NcurseView::rightDownSearch(){
    int x = cursorX;
    int y = cursorY;
    File file = vm.getFile();
    int line = firstDisplayLine+y;
    int max = min(screenW, file.getRawLineSize(line)+1);
    if (x + 1 >= max)
    { //if x is at the end of screen
        y++;
        x = 0;
    }
    else
    {
        x++;
    }
    return std::pair<int, int>(y, x);
}
std::pair<int,int> NcurseView::leftUpSearch(){
    int x = cursorX;
    int y = cursorY;
    File file = vm.getFile();
    if (x > 0)
        x--;
    else
    {
        int line = firstDisplayLine + y;
        if (line-1 >=0){
            y--;
            x = file.getRawLineSize(line - 1);
            if (x != 0){
                x--;
            }
        }
    }
    return std::pair<int, int>(y, x);
}
std::pair<int,int> NcurseView::moveUp(){
    int x = cursorX;
    int y = cursorY;
    if (validateY(y-1)){
        if (vm.getFile().getBeginIndexOnLine(y + firstDisplayLine) == 0){ //if current line is the begin of a line
            y--;
            while (validateY(y-1) &&vm.getFile().getBeginIndexOnLine(firstDisplayLine + y) != 0) //find the first beginindex line
            {
                y--;
            }

            if (!(validateX(y,x))){  //if we need to adjust x
                std::pair<int,int> data = vm.endLineCoord(y+firstDisplayLine); 
                if (data.first != -1){
                    x = data.second;
                    if (x != 0){
                        x--;
                    }
                    y = data.first - firstDisplayLine;
                }
            }
        }else{       //if current line isnt
            y--;
            while (validateY(y-1) &&vm.getFile().getBeginIndexOnLine(firstDisplayLine + y) != 0) //find top line
            {
                y--;
            }
            if (y + firstDisplayLine -1 < 0){ //if this line is at the end
                return std::pair<int,int>(cursorY,cursorX);
            }
            y--;
            while (y+firstDisplayLine >=0 &&vm.getFile().getBeginIndexOnLine(firstDisplayLine + y) != 0) //find the first beginindex line
            {
                y--;
            }
            if (!(validateX(y,x))){  //if we need to adjust x
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
    }



    // if (validateY(y-1)){
    //     y--;
    //     while (validateY(y-1) &&vm.getFile().getBeginIndexOnLine(firstDisplayLine + y) != 0)
    //     {
    //         y--;
    //     }

    //     if (!(validateX(y,x))){
    //         std::pair<int,int> data = vm.endLineCoord(y+firstDisplayLine); 
    //         if (data.first != -1){
    //             x = data.second;
    //             if (x != 0){
    //                 x--;
    //             }
    //             y = data.first - firstDisplayLine;
    //         }
    //     }
    // }
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
Action NcurseView::init(Action action){
    if (action == Action::queuePop)
    { //when popping a queue
        queue.pop_back();
    }
    else if (action != Action::redo && action != Action::repeatSearch && action != Action::repeatSearchOpp && action != Action::repeatCharSearch) //make a history
    {
        vm.pushActionToHistory(action);
    }
    if (!(queue.empty()) && !(focusedMode)){ //if queue is not needed 
        queue.clear();
    }
    return action;
}
void NcurseView::updateView(std::pair<int,Action> input){
    int c = input.first; //current command char
    std::pair<int, int> data;
    Action in = input.second;

    if (input.second == Action::await || input.second == Action::colonAwait || input.second == Action::searchAwait)
    { //when are waiting for a queue
        queue.push_back(c);
    }
    else
    { //else ready to do stuff
        in = init(input.second);
    }
    File &file = vm.getFile();           //easy file access
    int line = cursorY+firstDisplayLine; //line on
    int tmp; //to copy any int

    std::vector<int> info;
    std::string name;
    focusedMode = false;
    switch (in)
    {
    case Action::queuePop:
        focusedMode = true;
        break;
    case Action::invalid:
        queue.clear();
        break;
    case Action::await:
        break;
    case Action::moveToFileStart:
        cursorY = 0;
        cursorX = 0;
        firstDisplayLine = 0;
        queue.clear();
        break;
    case Action::insertOtherFile:
        for (auto ite = queue.begin() + 3; ite != queue.end(); ++ite)
        {
            name.push_back(static_cast<char>(*ite));
        }
        vm.insertOtherFile(line, name);
        updateMaxH();
        queue.clear();
        break;
    case Action::jumpToLine:
        cursorX = 0;
        cursorY = 0;
        data = vm.convertCursor(cursorY,cursorX);
        while (c != data.first){
            if (cursorY == moveDown().first){
                break;
            }
            cursorY = moveDown().first;
            data = vm.convertCursor(cursorY, cursorX);
        }
        queue.clear();
        break;
    case Action::repeatCharSearch:
        if (vm.getHistorySize() != 0 && (vm.getLastAction() == Action::moveToNextChar || vm.getLastAction() == Action::moveToPreviousChar)){
            if (vm.getLastAction() == Action::moveToNextChar){
                data = vm.nextCharCoord(line, cursorX, vm.getLastChar());
                if (data.first != -1){
                    cursorX = data.second;
                    cursorY = data.first - firstDisplayLine;
                }
                scrollDown();
            }else{
                data = vm.previousCharCoord(line, cursorX, vm.getLastChar());
                if (data.first != -1){
                    cursorX = data.second;
                    cursorY = data.first - firstDisplayLine;
                }
                scrollUp();
            }
        }
        break;
    case Action::repeatSearch:
        if (vm.getHistorySize() != 0){
            if (vm.getLastAction() == Action::search || vm.getLastAction() == Action::searchOpp){
                focusedMode = true; //retain a focused view
                std::vector<int> tmp = queue;
                tmp.erase(tmp.begin());
                if (vm.getLastAction() == Action::search){
                    data = rightDownSearch();
                    data = vm.searchFor(data.first + firstDisplayLine, data.second, tmp);
                }else{
                    data = leftUpSearch();
                    data = vm.searchForOpp(data.first + firstDisplayLine, data.second, tmp); //! change
                }
                if (data.first != -1){
                    cursorX = data.second;
                    cursorY = data.first - firstDisplayLine;
                }
            }
            scrollUp();
            scrollDown();
        }
        break;
    case Action::repeatSearchOpp:
        if (vm.getHistorySize() != 0){
            if (vm.getLastAction() == Action::search || vm.getLastAction() == Action::searchOpp){
                focusedMode = true; //retain a focused view
                std::vector<int> tmp = queue;
                tmp.erase(tmp.begin());
                if (vm.getLastAction() == Action::search){
                    data = leftUpSearch();
                    data = vm.searchForOpp(data.first + firstDisplayLine, data.second, tmp); //! change
                }else{
                    data = rightDownSearch(); //doesn't always work?
                    data = vm.searchFor(data.first + firstDisplayLine, data.second, tmp);
                }
                if (data.first != -1){
                    cursorX = data.second;
                    cursorY = data.first - firstDisplayLine;
                }
            }
            scrollUp();
            scrollDown();
        }
        break;
    case Action::searchOpp:
        if (!(queue.empty())){
            focusedMode = true;
            std::vector<int> tmp = queue;
            tmp.erase(tmp.begin());
            data = vm.searchForOpp(line, cursorX, tmp);
            if (data.first != -1){
                cursorX = data.second;
                cursorY = data.first - firstDisplayLine;
            }
            scrollUp();
            scrollDown();
        }
        break;
    case Action::search:
        if (!(queue.empty()))
        {
            focusedMode = true; //retain a focused view
            std::vector<int> tmp = queue;
            tmp.erase(tmp.begin());
            data = vm.searchFor(line, cursorX, tmp);
            if (data.first != -1){
                cursorX = data.second;
                cursorY = data.first - firstDisplayLine;
            }
            scrollUp();
            scrollDown();
        }
        break;
    case Action::colonAwait:
        focusedMode = true;
        break;
    case Action::searchAwait:
        focusedMode = true;
        break;
    case Action::saveNoExit:
        vm.saveFile();
        queue.clear();
        vm.getModified() = false;
        break;
    case Action::noSaveExit:
        if (!(vm.getModified())){
            endwin();
            vm.end();
            return;
        }
        break;
    case Action::moveToFileEnd:
        cursorX = 0;
        cursorY = 0;
        data = vm.convertCursor(cursorY,cursorX);
        while (maxH != data.first){
            if (cursorY == moveDown().first){
                break;
            }
            cursorY = moveDown().first;
            data = vm.convertCursor(cursorY, cursorX);
        }
        cursorX = file.getRawLineSize(cursorY + firstDisplayLine);
        if (cursorX != 0){
            cursorX--;
        }
        queue.clear();
        break;
    case Action::saveAndExit:
        vm.saveFile();
        endwin();
        vm.end();
        return;
        break;
    case Action::noSaveExitForce:
        endwin();
        vm.end();
        return;
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
    case Action::moveHalfScreenForward:
        showFileInfo = false;
        tmp = screenH/2;
        while (cursorY + firstDisplayLine + 1 < maxH&& tmp >=0)
        {
            if (firstDisplayLine + screenH < maxH){
                firstDisplayLine++;
            }else{
                cursorY++;
            }
            tmp--;
        }

        if (!(validateX(cursorY,cursorX))){
            cursorX = file.getRawLineSize(cursorY + firstDisplayLine);
            if (cursorX != 0){
                cursorX--;
            }
        }
        break;
    case Action::moveOneScreenForward:
        showFileInfo = false;
        tmp = screenH;
        while (cursorY + firstDisplayLine + 1 < maxH&& tmp >=0)
        {
            if (firstDisplayLine + screenH < maxH){
                firstDisplayLine++;
            }else{
                cursorY++;
            }
            tmp--;
        }

        if (!(validateX(cursorY,cursorX))){
            cursorX = file.getRawLineSize(cursorY + firstDisplayLine);
            if (cursorX != 0){
                cursorX--;
            }
        }
        break;
    case Action::moveHalfScreenBack:
        showFileInfo = false;
        tmp = screenH/2;
        while (cursorY+firstDisplayLine-1>= 0 && tmp >=0)
        {
            if (firstDisplayLine - 1 >= 0){
                firstDisplayLine--;
            }else{
                cursorY--;
            }

            tmp--;
        }
        if (!(validateX(cursorY,cursorX))){
            cursorX = file.getRawLineSize(cursorY + firstDisplayLine);
            if (cursorX != 0){
                cursorX--;
            }
        }
        break;
    case Action::moveOneScreenBack:
        showFileInfo = false;
        tmp = screenH;
        while (firstDisplayLine - 1 >= 0 && tmp >=0)
        {
            firstDisplayLine--;
            tmp--;
        }
        if (!(validateX(cursorY,cursorX))){
            cursorX = file.getRawLineSize(cursorY + firstDisplayLine);
            if (cursorX != 0){
                cursorX--;
            }
        }
        break;
    case Action::showFileStatus:
        showFileInfo = true;
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
        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
        break;
    case Action::deleteLeftNC:
        data = moveLeftUp();
        if (!(cursorX == data.second && cursorY == data.first)){
            vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
            cursorX = data.second;
            cursorY = data.first;
            vm.deleteCharSimple(line, cursorX,false);
        }
        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
        break;
    case Action::deleteRightNC:
        removeCharHelper(line, cursorX,false,input.second);
        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
        break;
    case Action::deleteUpNC:
        data = moveUp();
        if (!(cursorX == data.second && cursorY == data.first)){
            vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
            vm.removeLineFromFile(data.first + firstDisplayLine, false);
            vm.setSkipRecord(true);
            vm.removeLineFromFile(data.first + firstDisplayLine, false);
            updateMaxH();

            cursorX = 0;
            cursorY = data.first;
            while (!(validateY(cursorY))){
                cursorY--;
            }
        }
        scrollUp();
        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
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
        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
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
    case Action::redo:
        if (vm.getHistorySize() != 0){
            if (vm.getLastAction() == Action::moveToNextChar || vm.getLastAction() == Action::moveToPreviousChar){
                updateView(std::pair<int, Action>(input.first, Action::repeatCharSearch));
            }else{
                updateView(std::pair<int, Action>(input.first, vm.getLastAction()));
            }
            
            return;
        }
        break;
    case Action::deleteUp:
        data = moveUp();
        if (!(cursorX == data.second && cursorY == data.first)){
            vm.recordCursor(cursorY, firstDisplayLine, cursorX, maxH,input.second);
            vm.removeLineFromFile(data.first + firstDisplayLine, false);
            vm.setSkipRecord(true);
            vm.removeLineFromFile(data.first + firstDisplayLine, true);
            updateMaxH();

            cursorX = 0;
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
        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
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
        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
        break;
    case Action::toCommand:
        vm.setState(State::command);
        break;
    case Action::toInsert:
        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
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
        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
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
        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
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

        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
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
        vm.setStateHelper(cursorY,firstDisplayLine,cursorX,maxH,State::insert);
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
        showFileInfo = false;
        firstDisplayLine++;
        cursorY--;
    }
}

void NcurseView::scrollUp(){
    while (cursorY <= 4){
        if (firstDisplayLine-1 < 0){
            return;
        }
        showFileInfo = false;
        firstDisplayLine--;
        cursorY++;
    }
}

void NcurseView::updateView(int c){ //for INsert mode //! implement replace mode
    if (c != 27)
    {                  //NOT esecape key
    int line = cursorY + firstDisplayLine;
        if (c == 127){ //given delete key
            int beginIndex = vm.getFile().getBeginIndexOnLine(line);
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
    }
    erase();
    displayFile(); //display the original file
    displayStatusBar();
    moveCursor(cursorY, cursorX);
    refresh();
}

NcurseView::~NcurseView(){
}