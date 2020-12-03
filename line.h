#ifndef ___LINE___H
#define ___LINE___H

#include <vector>
#include "word.h"
class Line{
    std::vector<Word> line; //word processing vector
    std::vector<char> rawLine; //raw chars
    int lineSize;
    bool withNL;
public:
    Line(const std::vector<char>& line, bool withNL,int lineSize);
    Line(bool withNL,int lineSize);
    int insertCharAt(int x, int c);
    std::vector<char> &getRaw();
    bool isFull();
    void eraseCharAt(int x);
    void update();
    void print();
    int size();
    int rawSize();
    bool getWithNL();
    void setWithNL(bool given);
};

#endif
