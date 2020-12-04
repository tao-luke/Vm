#include "vm.h"
#include "ncurseView.h"
#include "keyboard.h"
#include "action.h"
#include <fstream>
#include <string>
#include <iostream>

void Vm::readFile(const char* name){
    try{    
        std::ifstream FILE(name);
        char c;
        while (FILE.get(c))
        {
            buffer.push_back(c);
        }

        FILE.close();
    }
    catch (...)
    {
        std::cout << "wow";
    }
}
void Vm::formatToFile(){
    file = std::move(std::make_unique<File>(buffer,lineSize));
}

Vm::Vm(const char* name,int lineSize):state{State::command},lineSize{lineSize}{
    readFile(name);
    formatToFile();
    addView(std::move(std::make_unique<NcurseView>(*this)));
    addControl(std::move(std::make_unique<Keyboard>()));
    //more funcs to come
}

void Vm::setState(State state){
    this->state = state;
}

State Vm::getState(){
    return state;
}
Vm::~Vm(){

}
int Vm::cursorToIndex(int y, int x){
    int index = 0;
    int counter = 0;
    for (auto &c : buffer)
    {
        if (y == 0 && x == 0) //end condition
            return index;
        if (c == '\n' || counter >= lineSize){ //y should be --
            y--;
            counter = 0;
        }
        else if (y == 0)
        {
            x--;
        }else{
            counter++;
        }
        index++;
    }
    std::cout << "inpossible cursor posn !!!!! WOWOWOW";
}

void Vm::formatToRaw(){
    buffer = std::move(file->convertToRaw());
}
void Vm::removeLine(int lineN){
    file->removeLine(lineN);
    formatToRaw();
}
void Vm::insertCharToFile(int y, int x, int c){
    file->insertCharToLine(y, x, c);
    formatToRaw();
}

void Vm::clearLine(int lineN){
    file->clearLine(lineN);
    formatToRaw();
}
void Vm::delCharFromFile(int y, int x){ //! add another version of this to support dd.
    if (file->charTotal() == 0)
        return;
    file->eraseCharFromLine(y, x);
    formatToRaw();
}

File &Vm::getFile(){
    return *file;
}
int Vm::getActionRaw(){
    return control->getActionRaw();
}

void Vm::run(){
    Action a;
    int b;
    if (view)
        displayView();
    while (true)
    {
        if (state == State::command){
            a = getAction();
            updateView(a);
        }
        else{
            b = getActionRaw();
            updateView(b);
        }
            
        
    }
}