#ifndef ___ACTION___H
#define ___ACTION___H
enum Action
{
    await,                         //next command is needed
    colonAwait,                    //await for colon mode
    searchAwait,                   //wait for search grrrr
    search,                        //  /
    searchOpp,                     //?
    repeatSearch,                  //   n
    repeatSearchOpp,               // N
    repeatCharSearch,              // ;
    left,                          //h
    right,                         //l
    up,                            //k
    down,                          //j
    toCommand,                     //esc one use of esc at least
    toInsert,                      //i
    joinThisAndNextWithSpace,      //J
    moveCursorToFrontThenInsert,   //I
    toFirstChar,                   //0
    deleteLine,                    //dd
    deleteLeft,                    //d <-
    deleteRight,                   // d ->
    deleteUp,                      //d up
    deleteDown,                    //d down
    deleteLeftNC,                  //c <-
    deleteRightNC,                 // c ->
    deleteUpNC,                    //c up
    deleteDownNC,                  //c down
    toInsertNext,                  // a
    toPreviousWord,                //b
    toNextWord,                    //w
    clearLine,                     // cc
    deleteChar,                    // x
    deleteCharThenInsert,          //s
    moveToNextChar,                //f(c)
    moveToPreviousChar,            // F(c)
    replaceCharWith,               // r(c)
    toReplace,                     //R
    undo,                          //u
    pasteAfterCursor,              //p
    pasteBeforeCursor,             //P
    copyCurrentLine,               //yy
    copyLeft,                      //y <-
    copyRight,                     // y ->
    copyUp,                        //y up
    copyDown,                      // y down
    moveCursorToLineEndThenInsert, // A
    insertNLUnderAndInsert,        //o
    insertNLAboveAndInsert,        //O
    toFirstNonBlank,               //^
    toLastChar,                    //$
    showFileStatus,                //^g
    redo,                          //.
    moveOneScreenBack,             //^b
    moveHalfScreenBack,            //^u
    moveHalfScreenForward,         //^d
    moveOneScreenForward,          //^f
    nothing,                       //no command
    invalid,                       //this should break!
    saveNoExit,                    //:w
    jumpToLine,                    //:(some number)
    noSaveExit,                    // :q!
    noSaveExitForce,               //:q!
    moveToFileStart,               //:0
    moveToFileEnd,                 //:$
    saveAndExit,                   //:wp
    insertOtherFile,               //:r
    queuePop,                      //when del in colonmode
};
#endif