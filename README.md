We all know how strangely competitive and subjective it can be sometimes when you share with your friends what IDE/code editor you prefer. While better-known brands now such as visual studio codes offer great customization and visuals, they are bulky. This leaves a portion of programmers who must work in limited space scenarios feeling unwelcomed. And while vim and nano are always an option, they are often considered by beginners as difficult with a long list of commands. Well, that arises a simple solution.... make your own text editor. Outlined here is my program VM, which follows the principle of simplicity of VIM with some loving personality changes. (arrow keys are primary here !!!:D )

This one man project is built using C++ and its ncurses library for graphics. 

Commands Supported:
a, b, x, c + motion, cc, f+character, d + motion, dd, h, k, l, i, n, o, p, r + character, s, u, w, x, y+ motion, yy, A, F + character, I, X, J, N, O, P, R, S, X, ^, $, 0, . , ;, /, ?, ctrl+b, ctrl+g, ctrl+d, ctrl+f, ctrl+u, :r, /search query, ?search query, :w, :q, :q!, :wq.

Number prefix IS allowed, so 5dd would remove 5 lines, 10u would undo 10 commands, and 9p would paste at cursor 9 times.

Search query is IS allowed with reverse search "?" available.

Syntax highlighting is supported for .cc and .h files with colors for:
  1) numeric literal
  2) string literal
  3) identifer
  4) comments
  5) C preprocessor 
  6) else

To run, simply type 

      make 
      
      ./vm (some file name)

