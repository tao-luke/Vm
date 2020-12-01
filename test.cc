#include "vm.h"
#include "ncurses.h"
int main(int argc, char** argv){
    initscr();
    int lineSize = getmaxx(stdscr);
    Vm test{argv[1],lineSize};
    test.run();
}



// #include "file.h"
// #include <vector>
// #include <iostream>
// #include <string>
// #include <fstream>

// int cursorToIndex(int y, int x, std::vector<char> buffer, int lineSize){
//     int index = 0;
//     int counter = 0;
//     for (auto &c : buffer)
//     {
//         if (y == 0 && x == 0) //end condition
//             return index;
//         if (c == '\n' || counter >= lineSize){ //y should be --
//             y--;
//             counter = 0;
//         }
//         else if (y == 0)
//         {
//             x--;
//         }else{
//             counter++;
//         }
//         index++;
//     }
//     std::cout << "inpossible cursor posn !!!!! WOWOWOW";
// }
// int main(){
//     std::vector<char> buffer;
//     std::vector<char> buffer2;
//     std::string content{"012\n\n012\n012"};

//     std::cout << "the original buffer vecctor is:" << std::endl;
//     for (auto n : content)
//     {
//         buffer.push_back(n);
//         int tmp = n;
//         std::cout << tmp << " ";
//     }
//     std::cout << std::endl;

//     File file{buffer, 3};
//     std::cout << "the lines printed from the original buffer vector" << std::endl;
//     file.printLines();

//     std::cout << "..... after removing line #0" << std::endl;
//     file.removeLine(0);
//     file.printLines();

//     buffer2 = std::move(file.convertToRaw());
//     std::cout << " after converting back to raw" << std::endl;
//     for (auto n : buffer2)
//     {
//         int tmp = n;
//         std::cout << tmp << " ";
//     }
//     std::cout << std::endl;

//     File file2{buffer2, 3};
//     std::cout << "and its file printed is:" << std::endl;
//     file.printLines();

//     int c = 9;
//     int index = cursorToIndex(1, 1, buffer2, 3);
//     auto ite = (buffer2.begin()) + index;
//     char ch = c;
//     buffer2.insert(ite, ch);

//     std::cout << " buffer2 after inserting 9 at pos 1,1" << std::endl;
//     for (auto n : buffer2)
//     {
//         int tmp = n;
//         std::cout << tmp << " ";
//     }
//     std::cout << std::endl;
//     File filef{buffer2, 3};
//     std::cout << "the FILE after inserting 9 at pos 1,1 using insertChar on buffer2" << std::endl;
//     filef.printLines();
    // std::cout << index << "hello" <<std::endl;
    // std::ifstream FILE("test1.txt");
    //     char c;
    //     std::vector<char> buffer{};
    //     int counter = 0;
    //     while (FILE.get(c))
    //     {
    //         buffer.push_back(c);
    //         std::cout << "wow" << c;
    //     }

    //     FILE.close();
    // File file{buffer, 10};
    // for (int i = 0; i < file.size();i++){
    //     for (auto c : file.getLineRaw(i))
    //     {
    //         std::cout << c;
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << file.size() << std::endl;
// }

// #include <ncurses.h>
// #include <string>
// int main(){
//     initscr();
//     noecho();
//     keypad(stdscr, TRUE);
//     int screenH, screenW;
//     getmaxyx(stdscr, screenH, screenW);

//     while (true){
//     int tmp;
//     tmp = getch();
//     std::string out = std::to_string(tmp);
//     char const *y = out.c_str();
//     printw(y);
//     printw("\n");
//     }
//     endwin();
// }