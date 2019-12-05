#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <err.h>
#include "header/arithmetic.h"
#include "header/astconvert.h"
#include "header/stack.h"
#include "../include/global.h"



void free_tree(struct tree *tree)
{
    if (tree)
    {
        free_tree(tree->left);
        free_tree(tree->right);
        free(tree->data);
        free(tree);
    }
}

void free_all(struct stack *operator, struct stack *operand)
{
    while (operator)
    {
        struct tree *tmp = operator->data;
        free_tree(tmp);
        operator = stack_pop(operator);        
    }
    while (operand)
    {
        struct tree *tmp = operand->data;
        free_tree(tmp);
        operand = stack_pop(operand);        
    }
}

struct tree *create_tree(char *data, enum type type)
{
    struct tree *new = malloc(sizeof(struct tree));

    if (new)
    {
        new->type = type;
        new->data = data;
        new->left = NULL;
        new->right = NULL;
    }

    return NULL;
}

struct tree *create_tree_lexer(void)
{
    struct tree *new = create_tree(lexer->head->value, NUMBER);
    struct token *out = pop_lexer();
    free(out);
    return new;
}

void combine(struct stack **operator, struct stack **operand)
{
    struct tree *node_operator = (*operator)->data;
    *operator = stack_pop(*operator);

    node_operator->right = (*operand)->data;
    *operand = stack_pop(*operand);

    node_operator->left = (*operand)->data;
    *operand = stack_pop(*operand);

    *operand = stack_push(*operand, node_operator);
}

struct tree *cancel(struct stack *operator, struct stack *operand)
{
    last_return_value = 1;
    free_all(operator, operand);
    if (lexer->head->primary_type == NUMBER)
        warnx("syntax error: operand expected");
    else
        warnx("syntax error: operator expected");
    return NULL;
}

void add_operand(struct stack **operand)
{
    struct tree *new = create_tree_lexer();
    *operand = stack_push(*operand, new);
}

int higher_priority(enum type type1, enum type type2)
{
    if ((type1 == ADD || type1 == SUB) && (type2 == MULT || type2 == DIV))
        return 1;
    return 0;
}

void add_operator(struct stack **operator, struct stack **operand)
{
    struct tree *top_operator = NULL;
    while (higher_priority((top_operator = (*operator)->data)->type,
    lexer->head->primary_type))
        combine(operator, operand);

    struct tree *new = create_tree_lexer();
    stack_push(*operator, new);
}

struct tree *result_tree(struct stack *operator, struct stack *operand)
{
    while (operator)
        combine(&operator, &operand);

    struct tree *result_tree = operand->data;
    free(operand);

    return result_tree;
}

//Higher priority on mult and sub
//Then other algebric operators
//Then Bitwise operator
struct tree *build_tree(void)
{
    struct stack *operand = NULL;
    struct stack *operator = NULL;

    int number_expected = 1;

    while (lexer->head)
    {
        if ((number_expected && lexer->head->primary_type != NUMBER)
            || (!number_expected && lexer->head->primary_type == NUMBER))
            return cancel(operator, operand);

        else if (lexer->head->primary_type == NUMBER)
            add_operand(&operand);
        
        else
            add_operator(&operator, &operand);
    }

    if (number_expected)
        return cancel(operator, operand);

    return result_tree(operator, operand);
}

int power(int a, int b)
{
    if (!b)
        return 1;

    int pos = (b > 0) ? 1 : 0;
    b = (b > 0) ? b : -b;
    int acc = a;

    for (int i = 0; i < b; i++)
    {
        if (pos)
            a *= acc;
        else
            a /= acc;
    }

    return a;
}

int match_val(enum type type, int left, int right)
{
    switch (type)
    {
    case ADD:
        return left + right;
    case SUB:
        return left - right;
    case MULT:
        return left * right;
    case DIV:
        return left / right;
    case POW:
        return power(left, right);
    default:
        warnx("Problem matching arithmetic expression with its value");
        return 0;
    }
}

int eval_tree(struct tree *tree)
{
    if (tree)
    {
        if (tree->type == NUMBER)
            return atoi(tree->data);
        return match_val(tree->type, eval_tree(tree->left),
            eval_tree(tree->right));
    }

    return 0;
} 
