#ifndef ___LINE___H
#define ___LINE___H

#include <vector>
#include "word.h"
#include <map>
class Line{
    std::vector<Word> line; //word processing vector
    std::vector<char> rawLine; //raw chars
    int lineLimit;
    int bufferLine;
    int fileLine;
    int beginIndex;
    bool withNL;
    int wordCount = 0;
    void convertToLine();
    void convertToRaw();
    std::map<int, int> indexToWord;

public:
    Line(const std::vector<char>& line, bool withNL,int lineSize,int fileLine,int bufferLine,int beginIndex);
    std::vector<char> &getRaw();
    bool isFull();
    int getPreviousWordCoord(int x);
    Word &getLastWord();
    void print();
    int size();
    int rawSize();
    bool getWithNL();
    int getFileLine();
    int getBufferLine();
    int getBeginIndex();
    void setWithNL(bool given);
};

#endif
