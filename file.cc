#include "file.h"
#include <iostream> //for testing

File::File(std::vector<char> raw, int lineSize)
{
    std::vector<char> tmp;
    int counter = 0;
    for(auto &c: raw){
        // int numb = c;
        // std::cout << "the counter is:" << counter << "and the c is:" << numb << std::endl;
        if (c == '\n'){ //if its newline
            if (!(tmp.empty())){ //if buffer isnt empty, push it in
                file.push_back(Line(tmp,true));
                tmp.clear();
                counter = 0;
            }else{
                file.push_back(Line(true));
            }
        }else if(counter >= lineSize){ //line size cap reached
            file.push_back(Line(tmp, false));
            tmp.clear();
            tmp.push_back(c);
            counter = 0;
            
        }else{ //if its a normal char, add it to buffer tmp
            tmp.push_back(c);
            counter++;
        }

    }
    if (!(tmp.empty())){ //if buffer isnt empty, push it in
        file.push_back(Line(tmp,false));
    }
}

std::vector<char> File::convertToRaw(){
    std::vector<char> tmp;
    for (auto &l: file){
        for (auto &c: l.getRaw()){
            tmp.push_back(c);
        }
        if (l.getWithNL())
            tmp.push_back('\n'); //! weird error with combination of dd and i??
    }
    return tmp;
}
int File::size(){
    return file.size();
}

std::vector<char>& File::getLineRaw(int line){
    return file[line].getRaw();
}
void File::clearLine(int line){

}
void File::removeLine(int line){
    auto ite = file.begin() + line;
    file.erase(ite);
}
int File::getLineSize(int line){
    return file[line].size();
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