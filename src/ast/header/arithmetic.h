#ifndef ARITHMETIC_H
#define ARITHMETIC_H

struct tree
{
    enum token_type type;
    char *data;
    struct tree *right;
    struct tree *left;
};

int eval_arith(void);

#endif
