#include <stddef.h>
#include <stdlib.h>
#include "header/stack.h"

struct stack *stack_push(struct stack *s, void *e)
{
    struct stack *new = malloc(sizeof(struct stack));
    if (new)
    {
        new->data = e;
        new->next = s;
        return new;
    }
    return s;
}

struct stack *stack_pop(struct stack *s)
{
    if (s)
    {
        struct stack *new = s->next;
        free(s);
        return new;
    }
    return NULL;
}

struct ast *stack_peek(struct stack *s)
{
    struct ast *out = s->data;
    return out;
}
