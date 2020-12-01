#ifndef ___LINE___H
#define ___LINE___H

#include <vector>
#include "word.h"
class Line{
    std::vector<Word> line; //word processing vector
    std::vector<char> rawLine; //raw chars
    bool withNL;
public:
    Line(const std::vector<char>& line, bool withNL);
    Line(bool withNL);
    std::vector<char> &getRaw();
    void update();
    void print();
    int size();
    int rawSize();
    bool getWithNL();
};

#endif
