/*!
 **  \file lexer.c
 **  \brief This file contains all the functions related to the lexer.
 **  \author 42sh Group
 */

#include "header/lexer.h"
#include "header/token.h"
#include "../auxiliary/header/auxiliary.h"

int is_separator(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len - 1)
    {
        return 0;
    }
    if (input[*index] == '&')
    {
        struct token *to_add = init_token(T_SEPARATOR, T_ANDIF,
        cut(input, index, *index + 2, len));
        add_token(lexer, to_add);
        *index += 2;
    }
    else if (input[*index] == '|')
    {
        struct token *to_add = init_token(T_SEPARATOR, T_ORIF,
        cut(input, index, *index + 2, len));
        add_token(lexer, to_add);
        *index += 2;
    }
    else
    {
        return 0;
    }
    return 1;
}

int is_if(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    if (tmp >= len - 1 || input[tmp] != 'i' || input[tmp + 1] != 'f'
        || input[tmp + 2] != ' ')
    {
        return 0;
    }
    tmp += 2;

    struct token *to_add = init_token(T_IF, T_WORD, cut(input, index, tmp,
    len));
    add_token(lexer, to_add);

    remove_white_space(input, &tmp, len);
    *index = tmp;

    is_command(input, index, len);

    return 1;
}

int is_then(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len - 3 || input[tmp] != 't' || input[tmp + 1] != 'h'
        || input[tmp + 2] != 'e' || input[tmp + 3] != 'n')
    {
        return 0;
    }
    tmp += 4;
    struct token *to_add = init_token(T_THEN, T_WORD,
        cut(input, index, tmp, len));
    add_token(lexer, to_add);
    to_add = NULL;

    *index = tmp;

    if (tmp == len)
    {
        return 1;
    }

    remove_white_space(input, &tmp, len);
    *index = tmp;

    which_separator(input, index, len);
    return 1;
}

int is_else(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len - 3 || input[tmp] != 'e' || input[tmp + 1] != 'l'
        || input[tmp + 2] != 's' || input[tmp + 3] != 'e')
    {
        return 0;
    }
    tmp += 4;
    struct token *to_add = init_token(T_ELSE, T_WORD,
        cut(input, index, tmp, len));
    add_token(lexer, to_add);
    to_add = NULL;

    if (tmp == len) // NEWLINE
    {
        *index = tmp;
        return 1;
    }

    remove_white_space(input, &tmp, len);
    *index = tmp;

    which_separator(input, index, len);
    return 1;
}

int is_elif(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len - 3 || input[tmp] != 'e' || input[tmp + 1] != 'l'
        || input[tmp + 2] != 'i' || input[tmp + 3] != 'f')
    {
        return 0;
    }
    tmp += 4;
    struct token *to_add = init_token(T_ELIF, T_WORD,
        cut(input, index, tmp, len));
    add_token(lexer, to_add);

    remove_white_space(input, &tmp, len);
    *index = tmp;

    is_command(input, index, len);

    return 1;
}

int is_fi(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len - 1 || input[tmp] != 'f' || input[tmp + 1] != 'i')
    {
        return 0;
    }
    struct token *to_add = init_token(T_FI, T_WORD, cut(input, &tmp, tmp + 2,
        len));
    add_token(lexer, to_add);
    *index += 2;
    if (tmp == len) // NEWLINE
        return 1;

    remove_white_space(input, index, len);;
    which_separator(input, index, len);
    return 1;
}

int is_command(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp == len)
        return 0;

    while (tmp < len && input[tmp] != ';')
    {
        tmp++;
        if (input[tmp] == ' ')
        {
            if (is_separator(input, &tmp, len))
            {
                struct token *to_add = init_token(T_COMMAND, T_NONE,
                    cut(input, index, tmp - 3, len));
                struct token *tmp2 = lexer->head;
                if (!tmp2->next)
                {
                    to_add->next = tmp2;
                    lexer->head = to_add;
                }
                else
                {
                    while (tmp2->next->next)
                    {
                        tmp2 = tmp2->next;
                    }
                    to_add->next = tmp2->next;
                    tmp2->next = to_add;
                }
                lexer->size += 1;
                remove_white_space(input, &tmp, len);
                *index = tmp;
            }
        }
    }
    if (tmp != *index)
    {
        struct token *to_add = init_token(T_COMMAND, T_NONE,
            cut(input, index, tmp, len));
        add_token(lexer, to_add);
    }
    remove_white_space(input, &tmp, len);
    *index = tmp;
    which_separator(input, index, len);
    return 1;
}

int which_separator(char *input, size_t *index, size_t len)
{
    struct token *to_add = NULL;

    if (input[*index] == ';')
    {
        to_add = init_token(T_SEPARATOR, T_SEMI,
                cut(input, index, *index + 1, len));
    }
    else if (input[*index] == '&')
    {
        to_add = init_token(T_SEPARATOR, T_AND,
                cut(input, index, *index + 1, len));
    }
    else if (input[*index] == '\n')
    {
        to_add = init_token(T_SEPARATOR, T_NEWLINE,
                cut(input, index, *index + 1, len));
    }
    if (to_add)
    {
        *index += 1;
        add_token(lexer, to_add);
        return 1;
    }
    return 0;
}

int is_comment(char *input, size_t *index, size_t len)
{
    if (*index == len)
        return 0;
    if (input[*index] == '#')
        return 1;
    return 0;
}

int is_while(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len - 4 || input[tmp] != 'w' || input[tmp + 1]  != 'h'
        || input[tmp + 2] != 'i' || input[tmp + 3] != 'l' || input[tmp + 4] !=
        'e')
    {
        return 0;
    }
    struct token *to_add = init_token(T_WHILE, T_WORD, cut(input, &tmp, tmp + 5,
        len));
    add_token(lexer, to_add);
    *index += 5;

    is_command(input, index, len);
    return 1;
}

int is_do(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if ((*index >= len - 1 || input[*index] != 'd'
            || input[*index + 1] != 'o') && (*index != len - 2
            || input[*index + 2] == 'n'))
    {
        return 0;
    }
    struct token *to_add = init_token(T_DO, T_WORD, cut(input, index,
        *index + 2, len));
    add_token(lexer, to_add);
    *index += 2;
    is_command(input, index, len);
    return 1;
}

int is_done(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len - 3 || input[*index] != 'd' || input[*index + 1] != 'o'
            || input[*index + 2] != 'n' || input[*index + 3] != 'e')
    {
        return 0;
    }
    struct token *to_add = init_token(T_DONE, T_WORD, cut(input, index,
        *index + 4, len));
    add_token(lexer, to_add);
    *index += 4;
    return 1;
}

int is_until(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len - 4 || input[*index] != 'u' || input[*index + 1] != 'n'
            || input[*index + 2] != 't' || input[*index + 3] != 'i'
            || input[*index + 4] != 'l')
    {
        return 0;
    }
    struct token *to_add = init_token(T_UNTIL, T_WORD, cut(input, index,
        *index + 5, len));
    add_token(lexer, to_add);
    *index += 5;
    return 1;
}
