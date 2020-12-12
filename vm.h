#ifndef ___VM___H
#define ___VM___H

#include "model.h"
#include <vector>
#include <memory>
#include "file.h"
#include "event.h"
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
    std::stack<Event> undoStack;
    std::stack<Action> history;
    const char *name;
    bool modified = false;
    bool inLinePaste = false;
    bool skipOneRecord = false;
    bool endIt = false;
    int lineSize = 0;
    std::vector<std::vector<char>> readFile(const char *name);
    void formatToFile();
    void formatToRaw();
    void mergeLines(int l1, int l2);
    void copyToClipboard(const std::vector<char> target,bool overide);
    bool validate(int vecline, int vecindex);
    std::pair<int, int> validateAndConvert(int line, int x);
    void clearLine(int first);
    bool fileExist(std::string &name);

public:
    void insertOtherFile(int line, std::string name);
    void end();
    void saveFile();
    size_t getHistorySize();
    Action getLastAction();
    void pushActionToHistory(Action action);
    bool& getModified();
    const char *getName();
    size_t getStackSize();
    const std::pair<int, int> convertCursor(int line, int x);
    void setSkipRecord(bool next);
    bool isUndoStackEmpty();
    void recordCursor(int cursorY, int firstDisplayLine, int cursorX, int maxH, Action action);
    void setStateHelper(int cursorY, int firstDisplayLine, int cursorX, int maxH, State state);
    int getMulti();
    const std::pair<int, int> pasteAfterCursor(int cursorY,int firstDisplayLine,int maxH,int cursorX);
    const std::pair<int, int> pasteBeforeCursor(int cursorY,int firstDisplayLine,int maxH, int cursorX);
    std::pair<int, int> endLineCoord(int lineN);
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
    std::vector<int> handleUndo();
    std::vector<std::vector<char>> &getBuffer();
};

#endif