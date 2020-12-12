// #include "vm.h"
// #include "ncurses.h"
// int main(int argc, char** argv){
//     initscr();
//     int lineSize = getmaxx(stdscr);
//     Vm test{argv[1],lineSize-3};
//     test.run();
// }


#include <ncurses.h>
#include <string>
int main(){
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    int screenH, screenW;
    getmaxyx(stdscr, screenH, screenW);
    start_color();
    init_pair(1,COLOR_YELLOW ,COLOR_BLACK);
    while (true)
    {
        int tmp;
        attron(COLOR_PAIR(1));
        tmp = getch();
        std::string out = std::to_string(tmp);
        char const *y = out.c_str();
        printw(y);
        printw("\n");
    }
    endwin();
}