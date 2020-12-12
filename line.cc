#include "line.h"
#include <iostream> //for testing
#include <ncurses.h>
#include <regex>
#define keyword 1
#define numLit 2
#define strLit 3
#define ident 4
#define comment 5
#define prep 6
#define other 7
Line::Line(const std::vector<char> &rawLine, bool withNL, int lineSize, int fileLine, int bufferLine, int beginIndex) : rawLine{rawLine}, lineLimit{lineSize}, bufferLine{bufferLine}, beginIndex{beginIndex}, withNL{withNL}
{
    convertToLine();
}
bool Line::isFull(){
    return lineLimit <= rawLine.size();
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

int Line::getNextWordCoord(int x){
    if (rawLine.size()== 0 || firstWordCoord() == -1){
        return 0;
    }
    for (int i = x + 1; i < rawLine.size(); i++)
    {
        try{
            indexToWord.at(i);
            return i;
        }
        catch (const std::out_of_range &oor)
        {
        }
    }
    return -1; //cant find the next word, 
}
int Line::firstSpaceCoord(){
    for (int i = 0; i < rawLine.size();i++){
        if (rawLine[i] == ' '){
            return i;
        }
    }
    return -1;
}

bool Line::isLinePrep(){
    if (firstWordCoord() != -1){
        return (rawLine[firstWordCoord()] == '#');
    }
    return false;
}
void Line::initColorPair(){
    init_pair(keyword,COLOR_MAGENTA ,COLOR_BLACK);
    init_pair(numLit,COLOR_GREEN ,COLOR_BLACK);
    init_pair(strLit, COLOR_YELLOW,COLOR_BLACK);
    init_pair(ident,COLOR_RED ,COLOR_BLACK);
    init_pair(comment,COLOR_CYAN ,COLOR_BLACK);
    init_pair(prep, COLOR_BLUE,COLOR_BLACK);
    init_pair(other, COLOR_WHITE,COLOR_BLACK);
}
void Line::displayLine(int y, int x)
{
    initColorPair(); //initialize the pairs for each type of word
    if (isLinePrep()){ //if this beigns with #
        for (auto &w:line){
            w.setColor(prep);
        }
    }else
    {
        if (firstCommentPos() != -1) //if there is a comment
        {
            for (auto start = line.begin() + firstCommentPos(); start != line.end();++start){ //set the parts after// as comment
                (*start).setColor(comment);
            }
            parseLine(0, firstCommentPos()); //parse the parts before //
        }else{
            parseLine(0,line.size());   //if not prep or comment, parse wholething
        }
    }

    for(auto & w:line){ //print words
        w.displayWord(y, x);
    }
}
int Line::firstCommentPos(){
    int counter = 0;
    for (auto &w : line)
    {
        if (w.getWord().size() >=2 && w.getWord().front() == '/' && w.getWord().at(1) == '/'){
            return counter;
        }
        counter++;
    }
    return -1;
}
void Line::parseLine(int start,int end){
    std::regex stringRule("^\".*\"$");
    std::regex numRule("^[0-9]+$");
    std::regex keywords("^(alignas)|(alignof)|(and)|(and_eq)|(asm)|(auto)|(bitand)|(bitor)|(bool)|(break)|(case)|(catch)|(char)|(char16_t)|(char32_t)|(class)|(compl)|(const)|(constexpr)|(const_cast)|(continue)|(decltype)|(default)|(delete)|(do)|(double)|(dynamic_cast)|(else)|(enum)|(explicit)|(export)|(extern)|(false)|(float)|(for)|(friend)|(goto)|(if)|(inline)|(int)|(long)|(mutable)|(namespace)|(new)|(noexcept)|(not)|(not_eq)|(nullptr)|(operator)|(or)|(or_eq)|(private)|(protected)|(public)|(register)|(reinterpret_cast)|(return)|(short)|(signed)|(sizeof)|(static)|(static_assert)|(static_cast)|(struct)|(switch)|(template)|(this)|(thread_local)|(throw)|(true)|(try)|(typedef)|(typeid)|(typename)|(union)|(unsigned)|(using)|(virtual)|(void)|(volatile)|(wchar_t)|(while)|(xor)|(xor_eq)^");
    std::regex types("^(int)|(char)|(bool)|(float)|(double)|(void)|(wchar_t)|(sample)$");
    for (int i = start; i < end; i++)
    {
        std::string copy = line[i].getWordString();
        if (std::regex_match(line[i].getWordString(), stringRule))
        { //if this word is a string
            line[i].setColor(strLit);
        }
        else if (std::regex_match(line[i].getWordString(), numRule))
        { //if num lit
            line[i].setColor(numLit);
        }
        else if (std::regex_match(line[i].getWordString(), keywords))
        { //key words
            line[i].setColor(keyword);
            if (std::regex_match(copy,types)){
                int next = nextWordNotSpace(++i);
                if (next != -1){
                    i = next;
                    line[i].setColor(ident);
                }
            }
        }
    }
}
int Line::firstWordCoord(){
    for (int i = 0; i < rawLine.size();i++){
        if (rawLine[i] != ' '){
            return i;
        }
    }
    return -1;
}
int Line::nextWordNotSpace(int x){
    while (x < line.size()){
        if (line[x].isSpace()){
            x++;
        }else{
            return x;
        }
    }
    return -1;
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