#include "../include/global.h"
#include "header/lexer.h"
#include "header/token.h"
#include "../prompt/header/prompt.h"
#include "../auxiliary/header/auxiliary.h"
#include "../include/include.h"
#include "../quoting/header/quoting.h"
#include "../ast/header/stringutils.h"

struct token *handle_bitwise(struct token *actual);
struct token *handle_negative(struct token *actual);
struct token *handle_bang(struct token *actual);

struct token *handle_positive(struct token *actual)
{
    if (!actual)
    {
        return NULL;
    }
    if (actual->primary_type == T_BITWISE)
    {
        actual = handle_bitwise(actual->next);
    }
    else if (actual->primary_type == T_BANG)
    {
        actual = handle_bang(actual->next);
    }
    else if (actual->primary_type == T_PLUS)
    {
        actual = handle_positive(actual->next);
    }
    else if (actual->primary_type == T_MINUS)
    {
        actual = handle_negative(actual);
    }
    return actual;
}

struct token *handle_negative(struct token *actual)
{
    if (!actual)
    {
        return NULL;
    }
    if (actual->primary_type == T_BITWISE)
    {
        actual = handle_bitwise(actual->next);
    }
    else if (actual->primary_type == T_BANG)
    {
        actual = handle_bang(actual->next);
    }
    else if (actual->primary_type == T_PLUS)
    {
        actual = handle_positive(actual->next);
    }
    else if (actual->primary_type == T_MINUS)
    {
        actual = handle_negative(actual->next);
    }
    if (actual->primary_type == T_NUMBER || actual->primary_type == T_NNUMBER)
    {
        int number = atoi(actual->value);
        printf("number pre * -1 %d\n", number);
        number = number * -1;
        printf("number post * -1 %d\n", number);
        free(actual->value);
        actual->value = calloc(sizeof(char), 100);
        actual->value = my_itoa(number, actual->value);
        actual->primary_type = number < 0 ? T_NNUMBER : T_NUMBER;
    }
    return actual;
}

struct token *handle_bang(struct token *actual)
{
    if (!actual)
    {
        return NULL;
    }
    if (actual->primary_type == T_BITWISE)
    {
        actual = handle_bitwise(actual->next);
    }
    else if (actual->primary_type == T_BANG)
    {
        actual = handle_bang(actual->next);
    }
    else if (actual->primary_type == T_PLUS)
    {
        actual = handle_positive(actual->next);
    }
    else if (actual->primary_type == T_MINUS)
    {
        actual = handle_negative(actual->next);
    }
    if (actual->primary_type == T_NUMBER || actual->primary_type == T_NNUMBER)
    {
        int number = atoi(actual->value);
        number = ! number;
        free(actual->value);
        actual->value = calloc(sizeof(char), 100);
        actual->value = my_itoa(number, actual->value);
    }

    return actual;
}

struct token *handle_bitwise(struct token *actual)
{
    if (!actual)
    {
        return NULL;
    }
    if (actual->primary_type == T_BITWISE)
    {
        actual = handle_bitwise(actual->next);
    }
    else if (actual->primary_type == T_BANG)
    {
        actual = handle_bang(actual->next);
    }
    else if (actual->primary_type == T_PLUS)
    {
        actual = handle_positive(actual->next);
    }
    else if (actual->primary_type == T_MINUS)
    {
        actual = handle_negative(actual->next);
    }
    if (actual->primary_type == T_NUMBER || actual->primary_type == T_NNUMBER)
    {
        int number = atoi(actual->value);
        number = ~ number;
        free(actual->value);
        actual->value = calloc(sizeof(char), 100);
        actual->value = my_itoa(number, actual->value);
    }

    return actual;
}

struct token *solo_operators(struct token *actual, int is_first)
{
    if (!actual)
    {
        return 0;
    }
    struct token *prev_token = actual;
    while (actual)
    {
        if (actual->primary_type == T_BITWISE)
        {
            actual = handle_bitwise(actual->next);
        }
        else if (actual->primary_type == T_BANG)
        {
            actual = handle_bang(actual->next);
        }
        else if (actual->primary_type == T_PLUS)
        {
            if (actual->next && actual->next->primary_type == T_NUMBER
                    && !is_first)
            {
                actual = actual->next->next;
                prev_token = actual;
                continue;
            }
            if (is_first)
            {
                prev_token = actual;
                actual = solo_operators(actual->next, 0);
            }
            else
                actual = handle_positive(actual->next);
        }
        else if (actual->primary_type == T_MINUS)
        {
            if (actual->next && actual->next->primary_type == T_NUMBER
                    && !is_first)
            {
                actual = actual->next->next;
                prev_token = actual;
                continue;
            }
            if (is_first)
            {
                prev_token = actual;
                actual = solo_operators(actual->next, 0);
            }
            else
                actual = handle_negative(actual->next);
        }
        else if (actual->primary_type == T_POWER
                || actual->primary_type == T_MULT
                || actual->primary_type == T_DIV
                || actual->primary_type == T_REM
                || actual->primary_type == T_MAND
                || actual->primary_type == T_MOR
                || actual->primary_type == T_MAND
                || actual->primary_type == T_XOR)
        {
            prev_token = actual;
            actual = solo_operators(actual->next, 0);
        }
        else
        {
            if ((actual->primary_type == T_NUMBER
                || actual->primary_type == T_NNUMBER)
                && !is_first)
            {
                return actual;
            }
            if (!(actual->primary_type == T_NUMBER
                || actual->primary_type == T_NNUMBER))
            {
                is_first = 1;
            }
            prev_token = actual;
            actual = actual->next;
            continue;
        }
        if (is_first)
        {
            struct token *tmp = prev_token->next;
            prev_token->next = actual;
            while (tmp != actual)
            {
                prev_token = tmp;
                tmp = tmp->next;
                free(prev_token->value);
                free(prev_token);
            }
            prev_token = actual;
        }
    }
    return NULL;
}
