#ifndef ___FILE___H
#define ___FILE___H

#include <vector>
#include "line.h"
#include <map>
class File{
    std::vector<Line> file;
    int lineSize;
    std::map<int, int> lineToRaw;

public:
    File(std::vector<std::vector<char>> buffer,int lineSize);
    int lineTotal();
    int getBeginIndexOnLine(int line);
    void printLines();
    std::vector<char>& getLineRaw(int line);
    int getLineSize(int line);
    int getRawLineSize(int line);
    std::vector<std::vector<char>> convertToRaw();
    void removeLine(int line);
    void LineJoinFromTo(int line1, int line2);
    bool isLineFull(int line);
    bool isLineEmpty(int line);
    int charTotal();
    std::pair<int, int> convert_cursor(int y, int x);
    bool getLineWithNL(int line);
    void setLineWithNL(int line,bool next);
    // void insertWord(int cursorY, int cursorX, const vector<char>& word);
    // void deleteFrom(int index);
    // int find(vector<char> word);
    // void join(int cursorY, int cursorX);
    // void deleteLine();
};

#endif
