#include "vm.h"
#include "ncurses.h"
int main(int argc, char** argv){
    initscr();
    int lineSize = getmaxx(stdscr);
    Vm test{argv[1],lineSize-3};
    test.run();
}



// #include <vector>
// #include <iostream>
// #include <string>
// #include <fstream>

// int main(){
//         std::ifstream FILE("test1.txt");
//         std::vector<std::vector<char>> buffer;
//         std::vector<char> tmp;
//         int counter = 0;
//         char c;
//         while (FILE.get(c))
//         {
//             if (c == '\n'){ //to apend a new line.
//                 buffer.push_back(tmp);
//                 tmp.clear();
//             }
//             else
//             {
//                 tmp.push_back(c);
//             }
//             counter++;
//         }
//         if (!(tmp.empty())){
//             buffer.push_back(tmp);
//         }
//         for (auto l: buffer){
//             for(auto c: l){
//                 std::cout << c;
//             }
//             std::cout<< "N" << std::endl;
//         }
//         std::cout << "char tot is: " << counter << std::endl;
//         FILE.close();
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