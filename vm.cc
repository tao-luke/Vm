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

std::pair<int,int> Vm::previousWordCoord(int line, int x){
    if (line < file->lineTotal()){ //if line is valid line
        int newIndex = file->getLine(line).getPreviousWordCoord(x);
        while (newIndex == 0 && file->getBeginIndexOnLine(line) != 0 && line > 0 && !(file->getLastWordOnLine(line-1).isSpace())){
            //repeat if we arrived at x=0 on a line, and it is NOT the beginning of raw line and its last line does NOT end with space
            line--;
            newIndex = file->getLine(line).getPreviousWordCoord(file->getRawLineSize(line));
        }
        return std::pair<int, int>(line, newIndex);
    }
    else
    {
        std::cout << "incorrect line given in picking previous word" << std::endl;
    }
}

void Vm::clearLine(int lineN){
    std::pair<int, int> data = file->convert_cursor(lineN, 0);
    if (data.first <= buffer.size()){
        buffer[data.first].clear();
    }
}
void Vm::mergeLines(int from, int to){ //add from to to and erase from
    if (from < buffer.size() && to < buffer.size()){
        for(auto &c:buffer[from]){
            buffer[to].push_back(c);
        }
        buffer.erase(buffer.begin() + from);
    }
    else
    {
        std::cout << "from and to are out of range" << std::endl;
    }
}
void Vm::delCharFromFile(int line, int x)
{
    std::pair<int, int> data = file->convert_cursor(line, x);
    int beginIndex = file->getLine(line).getBeginIndex();
    if (x==0){
        if (beginIndex ==0){
            if (data.first > 0){ //if there is a previous line to merge to
                mergeLines(data.first, data.first - 1);
            }
        }else{
            buffer[data.first].erase(buffer[data.first].begin() + data.second - 1);
        }
    }else{ // x-1's element
        buffer[data.first].erase(buffer[data.first].begin() + data.second - 1);
    }
    formatToFile();
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