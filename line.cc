#include "line.h"
#include <iostream> //for testing
Line::Line(const std::vector<char>& rawLine,bool withNL,int lineSize):rawLine{rawLine},withNL{withNL},lineSize{lineSize}{
    update();
}

Line::Line(bool withNL,int lineSize):withNL{withNL},lineSize{lineSize}{
    
}
bool Line::isFull(){
    return lineSize <= rawLine.size();
}
void Line::update(){
    std::vector<char> tmp;
    for(auto &c: rawLine){
        if (c == ' '){ //if its space
            if (!(tmp.empty())){ //if buffer isnt empty, push it in
                line.push_back(Word(tmp));
                tmp.clear();
            }
            line.push_back(Word()); //add a space
        }else{ //if its a normal char, add it to buffer tmp
            tmp.push_back(c);
        }
    }
    if (!(tmp.empty())) line.push_back(Word(tmp));
}
int Line::insertCharAt(int x, int c){
    int overflow = -1;
    if (rawSize() == lineSize)
    {
        overflow = rawLine[lineSize - 1];
        rawLine.pop_back();
    }
    auto ite = rawLine.begin() + x;
    rawLine.insert(ite, c);
    return overflow;
}
void Line::print(){ //! for debuggin
    // for(int i = 0;i<line.size();i++){
    //     if (line[i].isSpace()){
    //         std::cout << " space ";
    //     }else{
    //         for(auto n : line[i].getWord()){
    //             std::cout << n;
    //         }
    //     }
    // }
    //raw for now
    for (auto & c: rawLine){
        int tmp = c;
        std::cout << tmp << " ";
    }
}

bool Line::getWithNL(){
    return withNL;
}
void Line::setWithNL(bool given){
    withNL = given;
}
std::vector<char>& Line::getRaw(){
    return rawLine;
}

void Line::eraseCharAt(int x){
    if (x<= lineSize){
        auto ite = rawLine.begin() + (x-1);
        rawLine.erase(ite);
    }
}
int Line::size(){
    return line.size();
}

int Line::rawSize(){
    return rawLine.size();
}