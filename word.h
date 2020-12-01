#ifndef ___WORD___H
#define ___WORD___H
#include <vector>
class Word{
    std::vector<char> word;
    bool space = false;
    public:

    Word(std::vector<char> word):word{word}{};
    Word():word{std::move(std::vector<char>(' '))},space{true}{

    }
    bool operator==( Word& target){
        return (word == target.getWord());
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