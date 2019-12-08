#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
//#include "../lexer/header/token.h"
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

struct tree *create_tree(char *data, enum token_type type)
{
    struct tree *new = malloc(sizeof(struct tree));

    if (new)
    {
        new->type = type;
        new->data = data;
        new->left = NULL;
        new->right = NULL;
        return new;
    }

    return NULL;
}

struct tree *create_tree_lexer(void)
{
    struct tree *new = create_tree(lexer->head->value,
        lexer->head->primary_type);
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
    if (lexer->head->primary_type == T_NUMBER)
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

int higher_priority(enum token_type type1, enum token_type type2)
{
    /*if ((type1 == T_PLUS || type1 == T_MINUS)
        && (type2 == T_MULT || type2 == T_DIV))
        return 0;*/

    switch (type1)
    {
    case T_MULT:
    case T_DIV:
        return type2 != T_POWER;
    case T_PLUS:
    case T_MINUS:
        return type2 != T_MULT && type2 != T_DIV && type2 != T_POWER;
    case T_MOR:
    case T_XOR:
    case T_MAND:
        return type2 != T_PLUS && type2 != T_MINUS && type2 != T_POWER
            && type2 != T_MULT && type2 != T_DIV;
    case T_ANDIF:
    case T_ORIF:
        return type2 != T_PLUS && type2 != T_MINUS && type2 != T_POWER
            && type2 != T_MULT && type2 != T_DIV && type2 != T_MOR
            && type2 != T_MAND && type2 != T_XOR;
    default:
        return 1;
    }
    return 1;
}

void add_operator(struct stack **operator, struct stack **operand)
{
    struct tree *top_operator = NULL;
    while (*operator
        && higher_priority((top_operator = (*operator)->data)->type,
        lexer->head->primary_type))
        combine(operator, operand);

    struct tree *new = create_tree_lexer();
    *operator = stack_push(*operator, new);
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

    while (lexer->head && (lexer->head->primary_type == T_NUMBER
        || lexer->head->primary_type == T_NNUMBER
        || lexer->head->secondary_type == T_OPERATOR))
    {
        if ((number_expected && lexer->head->primary_type != T_NUMBER
            && lexer->head->primary_type != T_NNUMBER)
            || (!number_expected && lexer->head->primary_type == T_NUMBER))
            return cancel(operator, operand);

        else if (lexer->head->primary_type == T_NUMBER
            || lexer->head->primary_type == T_NNUMBER)
        {
            add_operand(&operand);
            number_expected = 0;
        }

        else
        {
            add_operator(&operator, &operand);
            number_expected = 1;
        }
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

int match_val(enum token_type type, int left, int right)
{
    switch (type)
    {
    case T_PLUS:
        return left + right;
    case T_MINUS:
        return left - right;
    case T_MULT:
        return left * right;
    case T_DIV:
        return left / right;
    case T_POWER:
        return power(left, right);
    case T_MOR:
        return (left | right);
    case T_ORIF:
        return (left || right);
    case T_MAND:
        return (left & right);
    case T_ANDIF:
        return (left && right);
    case T_XOR:
        return (left ^ right);
    default:
        warnx("Problem matching arithmetic expression with its value");
        return 0;
    }
}

int eval_tree(struct tree *tree)
{
    if (tree)
    {
        if (tree->type == T_NUMBER)
            return atoi(tree->data);
        if (tree->type == T_NNUMBER)
            return -atoi(tree->data);
        return match_val(tree->type, eval_tree(tree->left),
            eval_tree(tree->right));
    }

    return 0;
}

int eval_arith(void)
{
    struct tree *tree = build_tree();
    int out = eval_tree(tree);
    free_tree(tree);
    //printf("result: %d\n", out);
    //exit(0);
    return out;
}
