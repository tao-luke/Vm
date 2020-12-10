#ifndef ___ACTION___H
#define ___ACTION___H
enum Action
{
    await,      //next command is needed
    left,        //h
    right,       //l
    up,          //k
    down,        //j
    toCommand,   //esc one use of esc at least
    toInsert,    //i
    joinThisAndNextWithSpace, //J
    moveCursorToFrontThenInsert, //I
    deleteLine,  //dd
    deleteLeft,  //d <-
    deleteRight, // d ->
    deleteUp,    //d up
    deleteDown,  //d down
    toInsertNext, // a 
    toPreviousWord, //b
    toNextWord, //w
    clearLine, // cc
    deleteChar, // x
    deleteCharThenInsert,  //s
    moveToNextChar, //f(c)
    moveToPreviousChar, // F(c)
    replaceCharWith, // r(c)
    pasteAfterCursor, //p
    copyCurrentLine, //yy
    copyLeft,       //y <-
    copyRight,         // y ->
    copyUp,         //y up
    copyDown,          // u down
    moveCursorToLineEnd, // A
    insertNLUnderAndInsert, //o
    nothing, //no command
    invalid, //this should break!
};
#endif