#ifndef ___FILE___H
#define ___FILE___H

#include <vector>
#include "line.h"

class File{
    std::vector<Line> file;
    public:
    File(std::vector<char> raw, int lineSize);
    int size();
    void printLines();
    std::vector<char>& getLineRaw(int line);
    int getLineSize(int line);
    int getRawLineSize(int line);
    std::vector<char> convertToRaw();
    void removeLine(int line);
    void clearLine(int line);
    // void insertWord(int cursorY, int cursorX, const vector<char>& word);
    // void deleteFrom(int index);
    // int find(vector<char> word);
    // void join(int cursorY, int cursorX);
    // void deleteLine();
};

#endif