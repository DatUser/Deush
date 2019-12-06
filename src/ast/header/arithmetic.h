#ifndef ARITHMETIC_H
#define ARITHMETIC_H

enum type
{
    NUMBER,
    ADD,
    SUB,
    MULT,
    DIV,
    POW
};

struct tree
{
    enum type type;
    char *data;
    struct tree *right;
    struct tree *left;
};

#endif
