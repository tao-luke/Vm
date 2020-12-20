#include "keyboard.h"
#include "ncurses.h"
Keyboard::Keyboard(){ //default to command mode  
    keymap[259] = Action::up;
    keymap[260] = Action::left;
    keymap[258] = Action::down;
    keymap[261] = Action::right;
    keymap['k'] = Action::up;
    keymap['h'] = Action::left;
    keymap['l'] = Action::right;
    keymap['j'] = Action::down;
    keymap[2] = Action::moveOneScreenBack;
    keymap[4] = Action::moveHalfScreenForward;
    keymap[6] = Action::moveOneScreenForward;
    keymap[21] = Action::moveHalfScreenBack;
    keymap['i'] = Action::toInsert;
    keymap['a'] = Action::toInsertNext;
    keymap['b'] = Action::toPreviousWord;
    keymap['x'] = Action::deleteChar;
    keymap['X'] = Action::deleteLeft;
    keymap['s'] = Action::deleteCharThenInsert;
    keymap['o'] = Action::insertNLUnderAndInsert;
    keymap['p'] = Action::pasteAfterCursor;
    keymap['A'] = Action::moveCursorToLineEndThenInsert;
    keymap['w'] = Action::toNextWord;
    keymap['I'] = Action::moveCursorToFrontThenInsert;
    keymap['J'] = Action::joinThisAndNextWithSpace;
    keymap['O'] = Action::insertNLAboveAndInsert;
    keymap['P'] = Action::pasteBeforeCursor;
    keymap['S'] = Action::clearLine;
    keymap['^'] = Action::toFirstNonBlank;
    keymap['$'] = Action::toLastChar;
    keymap['u'] = Action::undo;
    keymap['.'] = Action::redo;
    keymap['R'] = Action::toReplace;
    keymap['n'] = Action::repeatSearch;
    keymap['N'] = Action::repeatSearchOpp;
    keymap[7] = Action::showFileStatus;
    keymap[';'] = Action::repeatCharSearch;
    int tmp[] = {'d', 'c', 'r', 'F', 'f', 'y', '0', ':', '1', '2', '3', '4', '5', '6', '7', '8', '9', '/', '?'};
    std::set<int> sample(tmp,tmp+19);
    awaitKey = std::move(sample);
}
Action Keyboard::getActionMovement(int com1, int movement){
    switch(movement){
        case 258: //down
            switch(com1){
                case 100: //first command is d
                    return Action::deleteDown;
                    break;
                //!more cases here
                case 121:
                    return Action::copyDown;
                    break;
                case 99:
                    return Action::deleteDownNC;
                    break;
                }
                break;
        case 259: //up
            switch (com1){
                case 100:
                    return Action::deleteUp;
                    break;
                case 121:
                    return Action::copyUp;
                    break;
                case 99:
                    return Action::deleteUpNC;
                    break;
                }
                break;
        case 260: //left
            switch (com1){
                case 100:
                    return Action::deleteLeft;
                    break;
                case 121:
                    return Action::copyLeft;
                    break;
                case 99:
                    return Action::deleteLeftNC;
                    break;
                }
            break;
        case 261: //right
            switch (com1){
                case 100:
                    return Action::deleteRight;
                    break;
                case 121:
                    return Action::copyRight;
                    break;
                case 99:
                    return Action::deleteRightNC;
                    break;
                }
            break;
        }
    return Action::invalid;
}
Action Keyboard::getActionHelper(int initial,int buffer){
    switch (initial) //check command
    {
    case 100: //d case
        if (buffer == 100){ //dd case
            return Action::deleteLine;
        }
        else //d+ motion
        {
            return getActionMovement(100, buffer);
        }
        break;
    case 99: //c case
        if (buffer == 99){ //cc case
            return Action::clearLine;
        }
        else //c+ motion
        {
            return getActionMovement(99, buffer);
        }
        break;
    case 121: //y case
        if (buffer == 121){ //yy case
            return Action::copyCurrentLine;
        }
        else //y+ motion
        {
            return getActionMovement(121, buffer);
        }
        break;
    case 102: //f case
        return Action::moveToNextChar;
        break;
    case 70: //F case
        return Action::moveToPreviousChar;
    case 114: //r case
        return Action::replaceCharWith;
        break;
    default: 
        return Action::invalid;
        break;
    } 
}
bool Keyboard::isQueueNumber(){
    if (queue.empty())
        return false;
    for (auto &e : queue)
    {
        if (e <'0' || e > '9'){
            return false;
        }
    }
    return true;
}
bool Keyboard::isNumber(int initial){
    return (initial >= '0' && initial <= '9');
}
bool Keyboard::isAwaitCom(int initial){
    return (initial != ':' && initial != '/' && initial != '?');
}
std::pair<int, Action> Keyboard::handleSearchAction(int initial){
    if (initial == 10){ //search the thing!!!!
        searchMode = false;
        queue.clear(); 
        if (opp)
        {
            opp = false;
            return std::pair<int, Action>(0, Action::searchOpp);
        }
        else
        {
            return std::pair<int, Action>(0, Action::search);
        }
    }
    else
    {
        if (initial== 127){ //backspace
            queue.pop_back();
            if (queue.empty()){
                searchMode = false;
            }
            return std::pair<int, Action>(0, Action::queuePop);
        } //we are still entering
        queue.push_back(initial);
        return std::pair<int, Action>(initial, Action::searchAwait);
    }
}
std::pair<int, Action> Keyboard::handleColonAction(int initial){
    if (initial == 10){
        colonMode = false;
        if (isQueueNumber())
        {
            if (queue.size() == 1 && queue.front() == '0'){ // :0
                queue.clear();
                return std::pair<int, Action>(0, Action::moveToFileStart);
            }
            std::string tmp;
            char ch;
            for (auto &c : queue) //push queue to string 
            {
                ch = c;
                tmp.push_back(ch);
            }
            int line = std::stoi(tmp);
            queue.clear();
            return std::pair<int, Action>(line, Action::jumpToLine); //:number
        }else if (!(queue.empty())&& queue.front() == 'r')
        {
            queue.clear();
            return std::pair<int, Action>(0, Action::insertOtherFile);
        }
        else if (queue.size() == 1)
        {
            if (queue.front() == 'w'){ //:w
                queue.clear();
                return std::pair<int, Action>(0, Action::saveNoExit);
            }else if (queue.front() == 'q'){ //:q
                queue.clear();
                return std::pair<int, Action>(0, Action::noSaveExit);
            }
            else if (queue.front() == '$')
            { //:$
                queue.clear();
                return std::pair<int, Action>(0, Action::moveToFileEnd);
            }
        }
        else if (queue.size() == 2)
        {
            if (queue.front() == 'w' && queue.back() == 'q'){ //:wq
                queue.clear();
                return std::pair<int, Action>(0, Action::saveAndExit);
            }else if (queue.front() == 'q' && queue.back() == '!'){ //:q!
                queue.clear();
                return std::pair<int, Action>(0, Action::noSaveExitForce);
            }
        }
        else
        { 
            queue.clear();
            return std::pair<int, Action>(-1, Action::invalid);
        }
    }
    else
    { //if its not over yet
        if (initial == 127){
            queue.pop_back();
            if (queue.empty()){
                colonMode = false;
            }
            
            return std::pair<int, Action>(0, Action::queuePop);
        }else
        {
            queue.push_back(initial);
            return std::pair<int,Action>(initial, Action::colonAwait);
        }
    }
}
std::pair<int,Action> Keyboard::getAction() {
    int initial= 0;
    initial = getch();
    if (colonMode){
        return handleColonAction(initial);
    }
    else if (searchMode)
    {
        return handleSearchAction(initial);
    }
    else if (queue.empty())
    {                                                  //if queue is clear
        if (awaitKey.find(initial) != awaitKey.end()){ //if we find its a await needed command
            if (!(isNumber(initial)) && isAwaitCom(initial))
            {
                queue.push_back(initial);
            }
            if(initial == ':'){ //colon mode assoc
                colonMode = true;
                return std::pair<int, Action>(initial, Action::colonAwait); 
            }
            if (initial == '/' || initial == '?'){ //search mode assoc
                searchMode = true;
                if (initial == '?'){
                    opp = true;
                }
                return std::pair<int, Action>(initial, Action::searchAwait);
            }
            return std::pair<int, Action>(initial, Action::await);
        } //if its a simple command
        try{
            return std::pair<int, Action>(initial, keymap.at(initial));
        }catch(...){
            return std::pair<int, Action>(-1, Action::invalid);
        }
    }
    else
    { //return combo command
        int tmp = queue.front();
        queue.clear();
        return std::pair<int, Action>(initial, getActionHelper(tmp, initial));
    }
}

int Keyboard::getActionRaw(){
    int a;
    a = getch();
    return a;
}

