#ifndef LOOP_H
#define LOOP_H

struct env
{
    int depth;      //depth of loop we're in
    int break_until;//depth of loop we want to reach
    int is_continue;//did we meet continue
    int is_break;   //did we meet break
};

extern struct env *env;

#endif
