#ifndef ___VM___H
#define ___VM___H

#include "model.h"
#include <vector>
#include <memory>
#include "file.h"
enum State
{
    command,
    insert
};
class Vm :public Model{
    //possibly store some state here.
    State state;
    std::vector<vector<char>> buffer;
    unique_ptr<File> file;
    int lineSize = 0;
    void readFile(const char *name);
    void formatToFile();
    void formatToRaw();

public:
    void clearLine(int y);
    Vm(const char* name,int lineSize);
    ~Vm();
    void run();
    int getActionRaw();
    void insert_newline_inFile(int line);
    void insertCharToFile(int line, int x, int c);
    void delCharFromFile(int line, int x);
    void setState(State state);
    State getState();
    void removeLineFromFile(int lineN);
    File &getFile();
};

#endif