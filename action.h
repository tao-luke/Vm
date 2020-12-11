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
    toFirstChar, //0
    deleteLine,  //dd
    deleteLeft,  //d <-
    deleteRight, // d ->
    deleteUp,    //d up
    deleteDown,  //d down
    deleteLeftNC,  //c <-
    deleteRightNC, // c ->
    deleteUpNC,    //c up
    deleteDownNC,  //c down
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
    pasteBeforeCursor, //P
    copyCurrentLine, //yy
    copyLeft,       //y <-
    copyRight,         // y ->
    copyUp,         //y up
    copyDown,          // y down
    moveCursorToLineEndThenInsert, // A
    insertNLUnderAndInsert, //o
    insertNLAboveAndInsert, //O
    toFirstNonBlank, //^
    toLastChar, //$
    nothing, //no command
    invalid, //this should break!
};
#endif