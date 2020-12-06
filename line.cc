#include "line.h"
#include <iostream> //for testing
Line::Line(const std::vector<char>& rawLine,bool withNL,int lineSize,int fileLine,int bufferLine, int beginIndex):rawLine{rawLine},withNL{withNL},lineSize{lineSize},bufferLine{bufferLine},beginIndex{beginIndex}{
    update();
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
int Line::getBeginIndex(){
    return beginIndex;
}
int Line::getBufferLine(){
    return bufferLine;
}
int Line::getFileLine(){
    return fileLine;
}
int Line::size(){
    return line.size();
}

int Line::rawSize(){
    return rawLine.size();
}