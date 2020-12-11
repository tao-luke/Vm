#include "vm.h"
#include "ncurseView.h"
#include "keyboard.h"
#include "action.h"
#include <fstream>
#include <string>
#include <iostream>
class badlineN{
};
void Vm::readFile(const char *name)
{ //! need to add check for last char being \n
    try{    
        std::ifstream FILE(name);
        std::vector<char> tmp;
        char c;
        while (FILE.get(c))
        {
            if (c == '\n'){ //to apend a new line.
                buffer.push_back(tmp);
                tmp.clear();
            }
            else
            {
                tmp.push_back(c);
            }
        }
        if (!(tmp.empty())){
            buffer.push_back(tmp);
        }
        FILE.close();
    }
    catch (...)
    {
        std::cout << "file read error";
    }
}
std::pair<int,int> Vm::validateAndConvert(int line, int x){
    std::pair<int, int> data = file->convert_cursor(line, x); 
    if (validate(data.first,data.second)) //bad case
        return data;
    throw badlineN();
}
void Vm::formatToFile(){
    file = std::move(std::make_unique<File>(buffer,lineSize));
}
Vm::Vm(const char* name,int lineSize):state{State::command},lineSize{lineSize}{
    
    readFile(name);

    formatToFile();
    addView(std::move(std::make_unique<NcurseView>(*this)));
    addControl(std::move(std::make_unique<Keyboard>()));
    //more funcs to come
}

void Vm::setState(State state){
    this->state = state;
    if (state == State::insert){
        //! start from here, make sure to save the entire buffer for this.
    }
}

State Vm::getState(){
    return state;
}
Vm::~Vm(){

}

void Vm::insertNLBehind(int line, int x){
    std::pair<int,int> data;
    try{
        data = validateAndConvert(line, x);
    }catch(...){
        return;
    }

    undoStack.top().setContent(std::vector<std::vector<char>>(),data); //prepare for undo

    buffer.insert(buffer.begin() + data.first + 1, std::vector<char>());
    formatToFile();
}

void Vm::insertNLAbove(int line,int x){
    std::pair<int,int> data;
    try{
        data = validateAndConvert(line, x);
    }catch(...){
        return;
    }

    undoStack.top().setContent(std::vector<std::vector<char>>(),data); //prepare for undo

    buffer.insert(buffer.begin() + data.first, std::vector<char>());
    formatToFile();
}

void Vm::formatToRaw(){
    buffer = std::move(file->convertToRaw());
}
void Vm::setSkipRecord(bool next){
    skipOneRecord = next;
}
void Vm::removeLineFromFile(int lineN, bool savePre){ 
    std::pair<int,int> data;
    try{
        data = validateAndConvert(lineN, 0);
    }catch(...){
        return;
    }
    if (!(skipOneRecord)){ //for commands that runs this twice, we dont need to update twice.
        std::vector<std::vector<char>> tmp;
        tmp.push_back(buffer[data.first]);
        undoStack.top().setContent(std::move(tmp),data); //prepare for undo
    }else{
        undoStack.top().contentPushback(buffer[data.first]);
        skipOneRecord = false;
    }

    copyToClipboard(buffer[data.first],savePre);
    inLinePaste = false; //we took a whole line
    if (file->lineTotal() == 1)
    {
        clearLine(data.first);
    }
    else
    {
        buffer.erase(buffer.begin() + data.first);
        formatToFile();
    }
}
std::pair<int,int> Vm::endLineCoord(int line){
    if (line >= file->lineTotal()){ //valid y
        return std::pair<int, int>(-1, -1);
    }
    while (line+1 < file->lineTotal() && file->getBeginIndexOnLine(line+1) != 0){
        //find the last line
        line++;
    }
    return std::pair<int, int>(line, file->getRawLineSize(line));
}
void Vm::copyLineFromFile(int lineN,bool keepPrev){
    std::pair<int,int> data;
    try{
        data = validateAndConvert(lineN, 0);
    }catch(...){
        return;
    }
    copyToClipboard(buffer[data.first],keepPrev);
    inLinePaste = false;
}

const std::pair<int,int> Vm::pasteAfterCursor(int cursorY,int firstDisplayLine,int maxH,int x){
    int line = firstDisplayLine + cursorY;
    std::pair<int, int> data;
    try{ //validate the pos given
        data = validateAndConvert(line, x);
    }catch(...){
        return std::pair<int, int>(-1, -1);
    }

    int newLine = line;
    if (clipboard.empty()){ //if nothing to paste
        return std::pair<int, int>(-1, -1);// 
    }

    recordCursor(cursorY, firstDisplayLine, x, maxH,Action::pasteAfterCursor);

    if (inLinePaste) //paste inline
    {
        std::vector<std::vector<char>> tmp;
        tmp.push_back(buffer[data.first]);
        undoStack.top().setContent(tmp,data,0); //prepare for undo
        auto ite = buffer[data.first].begin() + data.second;
        if (!(buffer[data.first].empty())){
            ite++;
        }else{
            x--;
        }
        for (auto &c : clipboard[0])
        {
            x++;
            ite = buffer[data.first].insert(ite, c);
            ++ite;
        }
    }
    else //paste after this line
    {
        undoStack.top().setContent(std::vector<std::vector<char>>(),data,clipboard.size()); //prepare for undo
        auto ite = buffer.begin() + data.first;
        newLine = line+1;
        while (newLine < file->lineTotal() && file->getBeginIndexOnLine(newLine) != 0){
            newLine++;
        }
        for (auto &l : clipboard)
        {
            ite++;
            ite = buffer.insert(ite, l);
        }
        x = 0;
    }
    formatToFile();
    return std::pair<int, int>(newLine, x);
}
const std::pair<int, int> Vm::pasteBeforeCursor(int cursorY,int firstDisplayLine,int maxH, int x){
    std::pair<int,int> data;
    int line = firstDisplayLine + cursorY;
    try{ //validate the pos given
        data = validateAndConvert(line, x);
    }catch(...){
        return std::pair<int, int>(-1, -1);
    }
    int newLine = line;
    if (clipboard.empty()){ //if nothing to paste
        return std::pair<int, int>(-1, -1);// 
    }

    recordCursor(cursorY, firstDisplayLine, x, maxH,Action::pasteBeforeCursor);


    if (inLinePaste) //paste inline
    {
        std::vector<std::vector<char>> tmp;
        tmp.push_back(buffer[data.first]);
        undoStack.top().setContent(tmp,data,0); //prepare for undo
        auto ite = buffer[data.first].begin() + data.second;
        for (auto &c : clipboard[0])
        {
            ite = buffer[data.first].insert(ite, c);
            ++ite;
        }
    }
    else //paste before this line
    {
        undoStack.top().setContent(std::vector<std::vector<char>>(),data,clipboard.size());
        auto ite = buffer.begin() + data.first;
        newLine = line;
        while (newLine >0 && file->getBeginIndexOnLine(newLine) != 0){
            newLine--;
        }

        for (auto &l : clipboard)
        {
            ite = buffer.insert(ite, l);
            ite++;
        }
        x = 0;
    }
    formatToFile();
    return std::pair<int, int>(newLine, x);
}

void Vm::copyToClipboard(const std::vector<char> target,bool savePrev){
    if (multi.empty() && !savePrev){
        clipboard.clear();
    }
    if (inLinePaste){ //if its inline, we put them all int he front vector of char
        if (clipboard.empty()){
            clipboard.push_back(std::vector<char>());
        }
        for(auto &c: target){
            clipboard[0].push_back(c);
        }
    }else{ //else if its a line
        clipboard.push_back(target);
    }

}
void Vm::copyChar(int line, int x){
    std::pair<int,int> data;
    try{
        data = validateAndConvert(line, x);
    }catch(...){
        return;
    }
    inLinePaste = true;
    std::vector<char> tmp;
    tmp.push_back(*(buffer[data.first].begin() + data.second));
    copyToClipboard(std::move(tmp), false);
}
void Vm::insertCharToFile(int y, int x, int c){ //convert y and x to buffer index and insert accdingly
    std::pair<int,int> data;
    try{
        data = validateAndConvert(y, x);
    }catch(...){
        return;
    }

    auto ite = buffer[data.first].begin() + data.second;
    std::vector<char> tmp;
    if (c == '\n')
    {
        for (auto start = buffer[data.first].begin(); start != ite;++start){ //copy part1
            tmp.push_back(*start);
        }
        buffer[data.first].erase(buffer[data.first].begin(), ite); //remove part 1 from part2
        buffer.insert(buffer.begin() + data.first, tmp); //add part 1 to a line before part 2
    }
    else
    {
        buffer[data.first].insert(ite, c);
    }
    formatToFile();
}
std::pair<int,int> Vm::previousWordCoord(int line, int x){
    if (line < file->lineTotal()){ //if line is valid line
        int newIndex = file->getLine(line).getPreviousWordCoord(x);
        while (newIndex == 0 && file->getBeginIndexOnLine(line) != 0 && line > 0 && !(file->getLastWordOnLine(line-1).isSpace())){
            //repeat if we arrived at x=0 on a line, and it is NOT the beginning of raw line and its last line does NOT end with space
            line--;
            newIndex = file->getLine(line).getPreviousWordCoord(file->getRawLineSize(line));
        }
        return std::pair<int, int>(line, newIndex);
    }
    else
    {
        std::cout << "incorrect line given in picking previous word" << std::endl;
    }
}
std::pair<int,int> Vm::nextWordCoord(int line,int x){
    if (line < file->lineTotal()){ //if line is valid line
        int newIndex = file->getLine(line).getNextWordCoord(x);
        while (newIndex == -1 && line+1 < file->lineTotal()){
            //while w need to check the next line
            if (!(file->getLine(line).getLastWord().isSpace()) && !(file->getLineWithNL(line))){
                //if line doesnt end with space and it does NOT mark the end of the line
                line++;
                int firstS = file->getLine(line).firstSpaceCoord();
                if (firstS == -1){
                    newIndex = -1;
                }else{
                    newIndex = file->getLine(line).getNextWordCoord(firstS);
                }

            }else{
                line++;
                newIndex = file->getLine(line).getNextWordCoord(-1);
            }
        }

        return std::pair<int, int>(line, newIndex);
    }
    else
    {
        std::cout << "incorrect line given in picking previous word" << std::endl;
    }
}
const std::pair<int,int> Vm::firstNonBlankCoord(int line, int x){
    while(line > 0 && file->getBeginIndexOnLine(line) != 0){
        line--;
    }
    x = file->getLine(line).firstWordCoord();
    while (line+1 <file->lineTotal()&& x == -1 && !(file->getLineWithNL(line))){
        line++;
        x = file->getLine(line).firstWordCoord();
    }
    if (x == -1){
        x = file->getRawLineSize(line);
        if (!(x == 0)){
            x--;
        }
    }
    return std::pair<int, int>(line, x);
}
bool Vm::validate(int vecline,int vecindex){
    if (vecline > buffer.size() || vecindex > buffer[vecline].size()){
        return false;
    }
    return true;
}
void Vm::deleteCharSimple(int line, int x,bool copyIt){
    std::pair<int,int> data;
    try{
        data = validateAndConvert(line, x);
    }catch(...){
        return;
    }

    std::vector<std::vector<char>> tmp;
    tmp.push_back(buffer[data.first]);
    undoStack.top().setContent(std::move(tmp),data); //prepare for undo

    auto ite = buffer[data.first].begin() + data.second;
    if (copyIt)
    {
        inLinePaste = true;
        std::vector<char> tmp;
        tmp.push_back(*ite);
        copyToClipboard(std::move(tmp),false);
    }
    buffer[data.first].erase(ite);
    formatToFile(); //! we can improve efficiency here
}
bool Vm::isUndoStackEmpty(){
    return (undoStack.empty());
}
void Vm::recordCursor(int cursorY,int firstDisplayLine, int cursorX, int maxH, Action action)
{
    undoStack.push(std::move(Event(cursorY, firstDisplayLine, cursorX, maxH, action, *this)));
}
void Vm::clearLine(int first){
    buffer[first].clear();
}

void Vm::clearLineWithFormat(int lineN){
    std::pair<int,int> data;
    try{
        data = validateAndConvert(lineN, 0);
    }catch(...){
        return;
    }
    
    std::vector<std::vector<char>> tmp;
    tmp.push_back(buffer[data.first]);
    undoStack.top().setContent(std::move(tmp),data); //prepare for undo

    clearLine(data.first);
    formatToFile();
}
int Vm::getMulti(){
    try{
        return std::stoi(multi);
    }catch(...){
        return 0;
    }
}
std::vector<int> Vm::handleUndo(){
    std::vector<int> tmp = undoStack.top().undo();
    formatToFile();
    undoStack.pop();
    return tmp;
}
void Vm::mergeLines(int from, int to){ //add from to to and erase from
    if (from < buffer.size() && to < buffer.size()){
        for(auto &c:buffer[from]){
            buffer[to].push_back(c);
        }
        buffer.erase(buffer.begin() + from);
    }
    else
    {
        std::cout << "from and to are out of range" << std::endl;
    }
}
const std::pair<int,int> Vm::joinThisAndNextWithSpace(int line, int x){
    //find line end, merge this and next(CHECK IF this line ends with space, if not insert one), return line end pos
    std::pair<int, int> endPos = endLineCoord(line);
    std::pair<int,int> data;
    try{
        data = validateAndConvert(line, x); //try to get the converted vector indexs
    }catch(...){
        return std::pair<int, int>(-1, -1); //if invalid cursor pos
    }
    if (data.first+1 >= buffer.size()){ //if no line to join to
        return std::pair<int, int>(-1, -1);
    }

    std::vector<std::vector<char>> tmp;
    tmp.push_back(buffer[data.first]);
    tmp.push_back(buffer[data.first + 1]);
    undoStack.top().setContent(std::move(tmp),data); //prepare for undo

    if (!(buffer[data.first].empty()))
    {                                          //if current line isnt empty, index +1
        if (buffer[data.first].back() != ' '){ //add a space if needed
            buffer[data.first].push_back(' ');
        }else{
            endPos.second--;
        }
    }
    auto ite = buffer[data.first + 1].begin(); //rid the second line of space at the front
    while (ite != buffer[data.first+1].end() && *ite == ' ')
    {
        ite = buffer[data.first + 1].erase(ite);
    }
    for (auto &c: buffer[data.first+1]){ //push back second line
        buffer[data.first].push_back(c);
    }
    buffer.erase(buffer.begin() + data.first+1);
    formatToFile();
    return endPos;
}
const std::pair<int,int> Vm::nextCharCoord(int line, int x, int c){ //find next char assuming line is valid
    while (true){ //as long as the line we are checking isnt the end of the vector line, we can continue to next
        for (auto start = file->getLineRaw(line).begin() + x + 1; start != file->getLineRaw(line).end();++start){
            x++;
            if (*start == c)
            {
                return std::pair<int, int>(line, x);
            }
        }

        line++;
        if (line >= file->lineTotal() || ((file->getLineWithNL(line)) && (file->getBeginIndexOnLine(line-1) != 0)))
        {
            break;
        }
        x = 0;
    }
    return std::pair<int,int>(-1,-1);
}
const std::pair<int,int> Vm::previousCharCoord(int line, int x, int c){
    while (true){ //x is at least 1
        for (auto start = file->getLineRaw(line).rbegin()+ (file->getRawLineSize(line)-x); start != file->getLineRaw(line).rend();++start){
            x--;
            if (*start == c)
            {
                return std::pair<int, int>(line, x);
            }
        }

        line--;
        if (line < 0 || file->getBeginIndexOnLine(line+1) == 0)
        {
            break;
        }
        x = file->getRawLineSize(line);
    }
    return std::pair<int,int>(-1,-1);
}
void Vm::replaceCharAt(int line, int x, int c){ //given x is ok.
    std::pair<int,int> data;
    try{
        data = validateAndConvert(line, x);
    }catch(...){
        return;
    }

    std::vector<std::vector<char>> tmp;
    tmp.push_back(buffer[data.first]);
    undoStack.top().setContent(std::move(tmp),data); //prepare for undo

    buffer[data.first].insert(buffer[data.first].erase(buffer[data.first].begin() + data.second),c);
    formatToFile();
}
void Vm::delCharFromFile(int line, int x)
{
    std::pair<int,int> data;
    try{
        data = validateAndConvert(line, x);
    }catch(...){
        return;
    }
    int beginIndex = file->getLine(line).getBeginIndex();
    if (x == 0)
    {
        if (beginIndex ==0){
            if (data.first > 0){ //if there is a previous line to merge to
                mergeLines(data.first, data.first - 1);
            }
        }else{
            buffer[data.first].erase(buffer[data.first].begin() + data.second - 1);
        }
    }
    else
    { // x-1's element
        buffer[data.first].erase(buffer[data.first].begin() + data.second - 1);
    }
    formatToFile();
}

File &Vm::getFile(){
    return *file;
}
std::vector<std::vector<char>>& Vm::getBuffer(){
    return buffer;
}
int Vm::getActionRaw(){
    return control->getActionRaw();
}

void Vm::run(){
    std::pair<int, Action> data;
    int b;
    if (view)
        displayView();
    while (true)
    {
        if (state == State::command){
            data = getAction();
            if (data.first >= '0' && data.first <= '9'){
                if (getMulti() == 0 && data.first == '0'){
                    data.second = Action::toFirstChar;
                }
                else
                {
                    multi.push_back(static_cast<char>(data.first));
                }
            }
            if (getMulti() != 0 && data.second != Action::await){
                int tmp = getMulti();
                clipboard.clear();
                for (int i = 0; i < tmp; i++)
                {
                    updateView(data);
                }
                multi.clear();
            }
            else
            {
                updateView(data);
            }
            view->refreshView();
        }
        else{
            b = getActionRaw();
            updateView(b);
        }
            
        
    }
}