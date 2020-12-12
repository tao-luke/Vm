#include "file.h"
#include <iostream> //for testing
#include <unistd.h>

File::File(std::vector<std::vector<char>> buffer,int lineSize):lineSize{lineSize}
{
    std::vector<char> tmp;
    std::vector<char> tmp2;
    int bufferLineC = 0;
    int indexC = 0;
    int fileLineC = 0;
    for (auto l : buffer)
    {
        if (l.size() > lineSize){
            indexC = 0;
            tmp = l; //copy line
            while (!(tmp.empty())){
                auto start = tmp.begin();
                int endIndex = std::min(lineSize, static_cast<int>(tmp.size()));
                auto end = start + (endIndex);
                int prevIndexC = indexC;
                for (; start != end; ++start)
                {
                    tmp2.push_back(*start);
                    indexC++;
                }
                tmp.erase(tmp.begin(),end);
                if (tmp.empty())
                    file.push_back(Line(tmp2, true,lineSize,fileLineC, bufferLineC,prevIndexC));
                else
                    file.push_back(Line(tmp2, false,lineSize,fileLineC, bufferLineC,prevIndexC));
                lineToRaw.insert(std::pair<int,int>(fileLineC++, bufferLineC));
                tmp2.clear();
            }
        }
        else
        { //if lis withint width
            file.push_back(Line(l, true,lineSize, fileLineC,bufferLineC,0));
            lineToRaw.insert(std::pair<int,int>(fileLineC++, bufferLineC));
        }
        bufferLineC++;
    }
}
std::vector<std::vector<char>> File::convertToRaw(){
    std::vector<char> tmp;
    std::vector<std::vector<char>> newbuffer;

    for (auto l: file){
        if (l.getWithNL()){
            if (tmp.empty()){
                newbuffer.push_back(l.getRaw());
            }else{
                for (auto &c: l.getRaw()){
                    tmp.push_back(c);
                }
                newbuffer.push_back(tmp);
                tmp.clear();
            }
        }else{
            for (auto &c: l.getRaw()){
                tmp.push_back(c);
            }
        }
    }
    return newbuffer;
}
int File::lineTotal(){
    return file.size();
}
int File::getBeginIndexOnLine(int line){
    return file[line].getBeginIndex();
}

Line & File::getLine(int line){
    if (line < file.size()){
        return file[line];
    }else{
        std::cout << "not valid line" << std::endl;
    }
}
std::pair<int,int> File::convert_cursor(int y, int x){
    int first;
    int second;
    try
    {
        first = lineToRaw.at(y);
        second = getBeginIndexOnLine(y) + x;
    }
    catch(...)
    {
        std::cout << "converting error" << std::endl;
        throw;
    }
    return std::pair<int, int>(first, second);
}

bool File::isLineFull(int line){
    if (line >= file.size())
        return false;
    return (getRawLineSize(line) == lineSize);
}
bool File::isLineEmpty(int line){
    if (line >= file.size())
        return false;
    return (getRawLineSize(line) == 0);
}
std::vector<char>& File::getLineRaw(int line){
    if (line < file.size()){
        return file[line].getRaw();
    }else{
        std::cout << "not valid line" << std::endl;
    }
}
int File::getLineSize(int line){
    return file[line].size();
}
bool File::getLineWithNL(int line){
    return file[line].getWithNL();
}
Word & File::getLastWordOnLine(int line){
    if (line < file.size()){
        return file[line].getLastWord();
    }
    else
    {
        std::cout << " not valid line number when picking last word on line" << std::endl;
    }
}
void File::setLineWithNL(int line,bool next){
    file[line].setWithNL(next);
}
int File::charTotal(){
    int counter = 0;
    for (auto l: file){
        counter += l.rawSize();
    }
}
int File::getRawLineSize(int line){
    return file[line].rawSize();
}
void File::printLines(){
    int x = 0;
    int y = 0;
    for (int i = 0; i < lineTotal(); i++)
    {
        file[i].displayLine(y, x);
        std::cout << std::endl;
        y++;
        x = 0;
    }
}