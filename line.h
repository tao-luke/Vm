#ifndef ___LINE___H
#define ___LINE___H

#include <vector>
#include "word.h"
#include <map>
class Line{
    std::vector<Word> line; //word processing vector
    std::vector<char> rawLine; //raw chars
    int lineLimit; //max size allowed
    int bufferLine; //buffer line number this file line is connected with
    int fileLine; //which line on the file this line is
    int beginIndex; //index of first char of the buffer line this line represents
    bool withNL; //does this lin has an enter ad the end
    int wordCount = 0;
    void convertToLine();
    void convertToRaw();
    std::map<int, int> indexToWord; //maps an index to word.

public:
    Line(const std::vector<char>& line, bool withNL,int lineSize,int fileLine,int bufferLine,int beginIndex);
    std::vector<char> &getRaw();
    bool isFull();
    int firstSpaceCoord();
    int firstWordCoord();
    int getPreviousWordCoord(int x);
    int getNextWordCoord(int x);
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
