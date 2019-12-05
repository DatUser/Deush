#ifndef STACK_H
#define STACK_H

struct stack
{
    void *data;
    struct stack *next;
};

struct stack *stack_pop(struct stack *s);
struct stack *stack_push(struct stack *s, void *e);

#endif
