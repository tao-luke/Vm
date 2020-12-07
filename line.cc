#include "line.h"
#include <iostream> //for testing
Line::Line(const std::vector<char>& rawLine,bool withNL,int lineSize,int fileLine,int bufferLine, int beginIndex):rawLine{rawLine},withNL{withNL},lineLimit{lineSize},bufferLine{bufferLine},beginIndex{beginIndex}{
    convertToLine();
}
bool Line::isFull(){
    return lineLimit <= rawLine.size();
}
void Line::convertToRaw(){

}
void Line::convertToLine(){
    int word = 0;
    int index = 0;
    std::vector<char> tmp;
    for(auto &c: rawLine){

        if (c == ' '){ //if its space
            if (!(tmp.empty())){ //if buffer isnt empty, push it in
                line.push_back(Word(tmp));
                wordCount++;
                indexToWord.insert(std::pair<int, int>(index-tmp.size(), word++));
                tmp.clear();
            }
            line.push_back(Word()); //add a space
        }else{ //if its a normal char, add it to buffer tmp
            tmp.push_back(c);
        }
        index++;
    }
    if (!(tmp.empty())) {
        line.push_back(Word(tmp));
        indexToWord.insert(std::pair<int, int>(index-tmp.size(), word));
        wordCount++;
    }
}

int Line::getPreviousWordCoord(int x){
    int newIndex = x;
    for (int i = x - 1; i >= 0; i--)
    {
        try{
            indexToWord.at(i);
            return i;
        }
        catch (const std::out_of_range &oor)
        {
        }
    }
    return newIndex;
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
    return wordCount;
}

int Line::rawSize(){
    return rawLine.size();
}
Word& Line::getLastWord(){
    return line.back();
}