#include "keyboard.h"
#include "ncurses.h"
Keyboard::Keyboard(){ //default to command mode  //!replace with proper hjkl lol
    keymap[259] = Action::up;
    keymap[260] = Action::left;
    keymap[258] = Action::down;
    keymap[261] = Action::right;
    keymap['i'] = Action::toInsert;
    keymap['a'] = Action::toInsertNext;
    keymap['b'] = Action::toPreviousWord;
    keymap['x'] = Action::deleteChar;
    keymap['s'] = Action::deleteCharThenInsert;
    keymap['o'] = Action::insertNLUnderAndInsert;
    keymap['p'] = Action::pasteAfterCursor;
    keymap['A'] = Action::moveCursorToLineEnd;
    keymap['w'] = Action::toNextWord;
    keymap['I'] = Action::moveCursorToFrontThenInsert;
    keymap['J'] = Action::joinThisAndNextWithSpace;
    int tmp[] = {'d', 'c', 'r', 'F', 'f', 'y', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    std::set<int> sample(tmp,tmp+16);
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
                }
                break;
        case 259: //up
            switch (com1){
                case 100:
                    return Action::deleteUp;
                    break;
                //!more cases here
                }
                break;
        case 260: //left
            switch (com1){
                case 100:
                    return Action::deleteLeft;
                    break;
                //!more cases here
                }
            break;
        case 261: //right
            switch (com1){
                case 100:
                    return Action::deleteRight;
                    break;
                //!more cases here
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

std::pair<int,Action> Keyboard::getAction() {
    int initial= 0;
    initial = getch();
    if(queue.empty()){ //if queue is clear
        if (awaitKey.find(initial) != awaitKey.end()){ //if we find its a await needed command
            if (!(initial >= '0' && initial <= '9')){
                queue.push_back(initial);
            }
            return std::pair<int, Action>(initial, Action::await);
        } //if its a simple command
        try{
            return std::pair<int,Action>(initial,keymap.at(initial));
        }catch(...){
            return std::pair<int, Action>(-1, Action::invalid);
        }
    }else{ //return combo command
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