#ifndef ___WORD___H
#define ___WORD___H
#include <vector>
#include <ncurses.h>
#include <string>
#define keyword 1
#define numLit 2
#define strLit 3
#define ident 4
#define comment 5
#define prep 6
#define other 7
class Word{
    std::vector<char> word;
    bool space = false;
    int color = 7;
    
public:
    Word(std::vector<char> word) : word{word} {};
    Word():word{std::move(std::vector<char>(' '))},space{true}{
    }
    void setColor(int x){
        color = x;
    }
    bool operator==( Word& target){
        return (word == target.getWord());
    }
    void displayWord(int &y, int& x){
        if (space){ //if this word is space
            move(y, x++);
            addch(' ');
        }
        else
        { //if this word is NOT space
            attron(COLOR_PAIR(color));
            for (auto &c : word)
            {
                move(y, x++);
                addch(c);
            }
            attroff(COLOR_PAIR(color));
        }
    }
    std::string getWordString(){
        std::string tmp;
        for (auto &c : word)
        {
            tmp.push_back(c);
        }
        return tmp;
    }
    std::vector<char> & getWord(){
        return word;
    }

    int size(){
        return word.size();
    }

    bool isSpace(){
        return space;
    }

};

#endif