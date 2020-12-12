#include "vm.h"
#include "ncurses.h"
int main(int argc, char** argv){
    initscr();
    int lineSize = getmaxx(stdscr);
    Vm test{argv[1],lineSize-3};
    test.run();
}


// #include <ncurses.h>
// #include <string>
// #include <regex>
// #include <vector>
// #include <iostream>
// using namespace std;
// vector<string> split(string s, regex r)
// {
//   vector<string> splits;
//   smatch m; // <-- need a match object

//   while (regex_search(s, m, r))  // <-- use it here to get the match
//   {
//     int split_on = m.position(); // <-- use the match position
//     splits.push_back(s.substr(0, split_on));
//     s = s.substr(split_on + m.length()); // <-- also, skip the whole match
//   }

//   if(!s.empty()) {
//     splits.push_back(s); // and there may be one last token at the end
//   }

//   return splits;
// }

// int main(){
//     std::string line1{"#include wow something // this should al be comments wow"};
//     std::regex commentRule("^//.*");
//     for (auto &w : split(line1, commentRule)){
//         cout << w << endl;
//     }
// }

// initscr();
// noecho();
// keypad(stdscr, TRUE);
// int screenH, screenW;
// getmaxyx(stdscr, screenH, screenW);
// start_color();
// init_pair(1,COLOR_YELLOW ,COLOR_BLACK);
// while (true)
// {
//     int tmp;
//     attron(COLOR_PAIR(1));
//     tmp = getch();
//     std::string out = std::to_string(tmp);
//     char const *y = out.c_str();
//     printw(y);
//     printw("\n");
// }
// endwin();