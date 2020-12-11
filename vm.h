#ifndef ___VM___H
#define ___VM___H

#include "model.h"
#include <vector>
#include <memory>
#include "file.h"
#include <utility>
#include <string>
#include <stack>
#include <set>
enum State
{
    command,
    insert,
};
class Vm :public Model{
    //possibly store some state here.
    State state;
    std::vector<vector<char>> buffer;
    unique_ptr<File> file;
    std::vector<std::vector<char>> clipboard;
    std::string multi;
    std::stack<Action> undoStack;
    bool inLinePaste = false;
    int lineSize = 0;
    void readFile(const char *name);
    void formatToFile();
    void formatToRaw();
    void mergeLines(int l1, int l2);
    void copyToClipboard(const std::vector<char> target,bool overide);
    bool validate(int vecline, int vecindex);
    std::pair<int, int> validateAndConvert(int line, int x);

public:
    int getMulti();
    const std::pair<int, int> pasteAfterCursor(int line, int x);
    const std::pair<int, int> pasteBeforeCursor(int line, int x);
    std::pair<int, int> endLineCoord(int lineN);
    void clearLine(int y);
    void clearLineWithFormat(int y);
    std::pair<int, int> previousWordCoord(int line, int x);
    std::pair<int, int> nextWordCoord(int line, int x);
    void replaceCharAt(int line, int x, int c);
    Vm(const char *name, int lineSize);
    ~Vm();
    const std::pair<int, int> joinThisAndNextWithSpace(int line, int x);
    const std::pair<int, int> nextCharCoord(int line, int x, int c);
    const std::pair<int, int> previousCharCoord(int line, int x, int c);
    const std::pair<int, int> firstNonBlankCoord(int line, int x);
    void deleteCharSimple(int line, int x, bool copyIt);
    void run();
    int getActionRaw();
    void insertNLBehind(int line, int x);
    void insertNLAbove(int line, int x);
    void insertCharToFile(int line, int x, int c);
    void delCharFromFile(int line, int x);
    void setState(State state);
    State getState();
    void removeLineFromFile(int lineN, bool copy);
    void copyLineFromFile(int lineN,bool override);
    void copyChar(int line, int x);
    File &getFile();
};

#endif