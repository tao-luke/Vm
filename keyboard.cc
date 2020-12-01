#include "keyboard.h"
#include "ncurses.h"
Keyboard::Keyboard(){ //default to command mode  //!replace with proper hjkl lol
    keymap[259] = Action::up;
    keymap[260] = Action::left;
    keymap[258] = Action::down;
    keymap[261] = Action::right;
    keymap['i'] = Action::toInsert;
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
    return Action::nothing;
}
Action Keyboard::getActionHelper(int initial){
    int buffer;
    switch (initial) //check command
    {
    case 100: //d case
        buffer = getch();
        if (buffer == 100){ //dd case
            return Action::deleteLine;
        }
        else //d+ motion
        {
            return getActionMovement(100, buffer);
        }
        break;
        
    default: //simple command
        return keymap.at(initial);
        break;
    } //not a command we have
    return Action::nothing;
}

Action Keyboard::getAction() {
    int initial= 0;
    Action a;
    initial = getch();
    try{
        a = getActionHelper(initial);
    }
    catch (...)
    {
        return Action::invalid;
    }
    return a; 
}

int Keyboard::getActionRaw(){
    int a;
    a = getch();
    return a;
}