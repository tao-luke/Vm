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
    int lineTotal(); //acc
    int getBeginIndexOnLine(int line); 
    void printLines();
    std::vector<char>& getLineRaw(int line);
    Line &getLine(int line);
    int getLineSize(int line);
    int getRawLineSize(int line);
    Word &getLastWordOnLine(int line);
    std::vector<std::vector<char>> convertToRaw();
    bool isLineFull(int line);
    bool isLineEmpty(int line);
    int charTotal();
    std::pair<int, int> convert_cursor(int y, int x);
    bool getLineWithNL(int line);
    void setLineWithNL(int line,bool next);
};

#endif
