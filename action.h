#ifndef ___ACTION___H
#define ___ACTION___H
enum Action
{
    left, //h
    right,  //l
    up, //k
    down,  //j
    toCommand, //esc one use of esc at least
    toInsert, //i
    deleteLine, //dd
    deleteLeft, //d <-
    deleteRight,// d ->
    deleteUp, //d up
    deleteDown, //d down

    nothing, //no command
    invalid, //this should break!
};
#endif