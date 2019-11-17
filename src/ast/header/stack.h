#ifndef STACK_H
#define STACK_H

#include "astconvert.h"

struct stack
{
    struct ast *data;
    struct stack *next;
};

struct stack *stack_pop(struct stack *s);

#endif
