#include "vm.h"
#include "ncurseView.h"
#include "keyboard.h"
#include "action.h"
#include <fstream>
#include <string>
#include <iostream>

void Vm::readFile(const char* name){ //! need to add check for last char being \n
    try{    
        std::ifstream FILE(name);
        std::vector<char> tmp;
        char c;
        while (FILE.get(c))
        {
            if (c == '\n'){ //to apend a new line.
                buffer.push_back(tmp);
                tmp.clear();
            }
            else
            {
                tmp.push_back(c);
            }
        }
        if (!(tmp.empty())){
            buffer.push_back(tmp);
        }
        FILE.close();
    }
    catch (...)
    {
        std::cout << "file read error";
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

void Vm::insert_newline_inFile(int line){
    std::pair<int, int> data = file->convert_cursor(line, 0);
    auto ite = buffer.begin() + data.first;
    buffer.insert(ite, std::vector<char>{});
    formatToFile();
}
void Vm::formatToRaw(){
    buffer = std::move(file->convertToRaw());
}
void Vm::removeLineFromFile(int lineN){ 
    std::pair<int, int> data = file->convert_cursor(lineN, 0);
    buffer.erase(buffer.begin() + data.first);
    formatToFile();
}
void Vm::insertCharToFile(int y, int x, int c){ //convert y and x to buffer index and insert accdingly
    std::pair<int, int> data = file->convert_cursor(y, x);
    auto ite = buffer[data.first].begin() + data.second;
    std::vector<char> tmp;
    if (c == '\n')
    {
        for (auto start = buffer[data.first].begin(); start != ite;++start){ //copy part1
            tmp.push_back(*start);
        }
        buffer[data.first].erase(buffer[data.first].begin(), ite); //remove part 1 from part2
        buffer.insert(buffer.begin() + data.first, tmp); //add part 1 to a line before part 2
    }
    else
    {
        buffer[data.first].insert(ite, c);
    }
    formatToFile();
}

void Vm::clearLine(int lineN){
    std::pair<int, int> data = file->convert_cursor(lineN, 0);
    if (data.first <= buffer.size()){
        buffer[data.first].clear();
    }
}
void Vm::delCharFromFile(int y, int x){ //! add another version of this to support dd.
    // if (file->charTotal() == 0)
    //     return;
    // file->eraseCharFromLine(y, x);
    // formatToRaw();
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