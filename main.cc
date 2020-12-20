#include "vm.h"
#include "ncurses.h"
int main(int argc, char** argv){
    initscr();
    int lineSize = getmaxx(stdscr);
    Vm test{argv[1],lineSize};
    test.run();
}

