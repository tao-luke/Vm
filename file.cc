#include "file.h"
#include <iostream> //for testing

File::File(std::vector<char> raw, int lineSize):lineSize{lineSize}
{
    std::vector<char> tmp;
    int counter = 0;
    for(auto &c: raw){
        // int numb = c;
        // std::cout << "the counter is:" << counter << "and the c is:" << numb << std::endl;
        if (c == '\n'){ //if its newline
            if (!(tmp.empty())){ //if buffer isnt empty, push it in
                file.push_back(Line(tmp,true,lineSize));
                tmp.clear();
                counter = 0;
            }else{
                file.push_back(Line(true,lineSize));
            }
        }else if(counter >= lineSize){ //line size cap reached
            file.push_back(Line(tmp, false,lineSize));
            tmp.clear();
            tmp.push_back(c);
            counter = 0;
            
        }else{ //if its a normal char, add it to buffer tmp
            tmp.push_back(c);
            counter++;
        }

    }
    if (!(tmp.empty())){ //if buffer isnt empty, push it in
        file.push_back(Line(tmp,false,lineSize));
    }
}
void File::insertCharToLine(int line, int x, int c){ //! enter doesnt work and fristdisplayline isnt shifting down

    if (c == '\n')
    {
        if (line +1 >= file.size()){ //if the current line is at the end
            std::vector<char> tmp;
            auto ite = ((file[line]).getRaw()).begin() + x; //find where we are adding nl
            auto start = ite;
            auto end = ((file[line]).getRaw()).end();
            while (start != end) //copy everything after newline to another line
            {
                tmp.push_back(*start);
                ++start;
            }
            ((file[line]).getRaw()).erase(ite,end); //erase the stuff from the original
            file.push_back(Line(tmp, true, lineSize)); //add a new line to the bottom with the moved chars
            file[line].setWithNL(true);
            return;
        }
        else 
        {
            file[line].setWithNL(true);
            std::vector<char> tmp;
            auto ite = ((file[line]).getRaw()).begin() + x; //find where we are adding nl
            auto start = ite;
            auto end = ((file[line]).getRaw()).end();
            while (start != end) //copy everything after newline to another line
            {
                tmp.push_back(*start);
                ++start;
            }
            ((file[line]).getRaw()).erase(ite,end);
            file.insert(file.begin()+ (line+1),Line(tmp, true, lineSize));
            return;
        }
    }
    else
    {
        int overflow = file[line].insertCharAt(x, c);
        while (overflow != -1)
        {
            if (line + 1 >= file.size())
            {                                             //if the current line is at the end
                file.push_back(Line(false, lineSize));    //add a new line to the bottom
                file[line + 1].insertCharAt(0, overflow); //pass int he overflown char
                if (file[line].getWithNL())               //if the original ine had NL pass it on but remove from old
                {
                    file[line + 1].setWithNL(true);
                    file[line].setWithNL(false);
                }
                break;
            }
            if (file[line].getWithNL())
            {                                //if the original line had  a NL
                file[line].setWithNL(false); //set this one to false NL
                auto ite = file.begin() + (line + 1);
                file.insert(ite, Line(true, lineSize)); //make the new line have NL
                file[line + 1].insertCharAt(0, overflow);
                break;
            }
            else
            {
                overflow = file[++line].insertCharAt(0, overflow);
            }
        }
    }
}
std::vector<char> File::convertToRaw(){
    std::vector<char> tmp;
    for (auto &l: file){
        for (auto &c: l.getRaw()){
            tmp.push_back(c);
        }
        if (l.getWithNL())
            tmp.push_back('\n'); 
    }
    return tmp;
}
int File::size(){
    return file.size();
}
void File::LineJoinFromTo(int from, int to){

    if (from <= file.size() && to <= file.size()){
        if (isLineEmpty(from)){
            removeLine(from);
            return;
        }
        else if (isLineEmpty(to)){
            removeLine(to);
            return;
        }
            int spots = lineSize - file[to].rawSize();
        for (int i = 0; i < spots;i++){
            if (file[from].rawSize() == 0){
                setLineWithNL(to, getLineWithNL(from));
                removeLine(from);
                break;
            }
            file[to].getRaw().push_back(file[from].getRaw().front());
            file[from].getRaw().erase(file[from].getRaw().begin());
        }
    }
}
void File::eraseCharFromLine(int line, int x){
    if (isLineEmpty(line)){
        auto ite = file.begin() + line;
        file.erase(ite);
    }else
    {
        if (x ==0){ //if we need to join
            if (getLineWithNL(line-1)){
                LineJoinFromTo(line, line - 1);
                recurTakeBottomUp(line);
            }
            else
            {
                file[line - 1].getRaw().pop_back();
                recurTakeBottomUp(line - 1);
            }
        }
        else
        { //simple case
            file[line].eraseCharAt(x);
            recurTakeBottomUp(line);
        }
    }
}


void File::recurTakeBottomUp(int from){
    if (!(getLineWithNL(from))){
        LineJoinFromTo(from+1, from );
        recurTakeBottomUp(from+ 1);
    }
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
    return file[line].getRaw();
}
void File::clearLine(int line){
    auto ite = file.begin() + line;
    auto newIte = file.erase(ite);
    file.insert(newIte, Line(true,lineSize));
}
void File::removeLine(int line){
    auto ite = file.begin() + line;
    file.erase(ite);
}
int File::getLineSize(int line){
    return file[line].size();
}
void File::insertNL(int line,bool withNL){
    auto ite = file.begin() + line;
    file.insert(ite,Line(withNL, lineSize));
}
bool File::getLineWithNL(int line){
    return file[line].getWithNL();
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
    for(int i=0;i<size();i++){
        file[i].print();
        std::cout << std::endl;
    }
}