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
    struct token *to_add_bis = NULL;
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

    while (tmp < len && input[tmp] != '\n') //separators
    {
        tmp++;
        if (input[tmp] == ' ')
        {
            //if (!is_separator(input, &tmp, len))
            //{
            //    continue;
            //}
            remove_white_space(input, &tmp, len);
        }
        if (input[tmp] == ';')
        {
            to_add_bis = init_token(T_SEPARATOR, T_SEMI,
            cut(input, &tmp, tmp + 1, len));
            break;
        }
        if (input[tmp] == '&')
        {
            to_add_bis = init_token(T_SEPARATOR, T_AND,
            cut(input, &tmp, tmp + 1, len));
            break;
        }
    }
    to_add = init_token(T_COMMAND, T_NONE, cut(input, index, tmp, len));
    add_token(lexer, to_add);
    if (to_add_bis)
    {
        add_token(lexer, to_add_bis);
        tmp++;
    }
    *index = tmp;
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

    if (input[tmp] == ';')
    {
        to_add = init_token(T_SEPARATOR, T_SEMI,
                cut(input, &tmp, tmp + 1, len));
    }
    else if (input[tmp] == '&')
    {
        to_add = init_token(T_SEPARATOR, T_AND,
                cut(input, &tmp, tmp + 1, len));
    }
    if (to_add)
    {
        add_token(lexer, to_add);
        *index += 1;
    }
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

    if (input[tmp] == ';')
    {
        to_add = init_token(T_SEPARATOR, T_SEMI,
                cut(input, &tmp, tmp + 1, len));
    }
    else if (input[tmp] == '&')
    {
        to_add = init_token(T_SEPARATOR, T_AND,
                cut(input, &tmp, tmp + 1, len));
    }
    if (to_add)
    {
        *index += 1;
        add_token(lexer, to_add);
    }
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
    struct token *to_add_bis = NULL;
    struct token *to_add = init_token(T_ELIF, T_WORD,
        cut(input, index, tmp, len));
    add_token(lexer, to_add);

    remove_white_space(input, &tmp, len);
    *index = tmp;

    while (tmp < len && input[tmp] != '\n') //separators
    {
        tmp++;
        if (input[tmp] == ' ')
        {
            if (!is_separator(input, &tmp, len))
            {
                continue;
            }
        }
        else if (input[tmp] == ';')
        {
            to_add_bis = init_token(T_SEPARATOR, T_SEMI,
            cut(input, &tmp, tmp + 1, len));
            break;
        }
        else if (input[tmp] == '&')
        {
            to_add_bis = init_token(T_SEPARATOR, T_AND,
            cut(input, &tmp, tmp + 1, len));
            break;
        }
    }

    to_add = init_token(T_COMMAND, T_NONE, cut(input, index, tmp, len));
    add_token(lexer, to_add);
    if (to_add_bis)
    {
        add_token(lexer, to_add_bis);
        tmp++;
    }
    *index = tmp;
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
    struct token *to_add = NULL;
    if (tmp == len) // NEWLINE
        return 1;

    remove_white_space(input, &tmp, len);
    *index = tmp;

    if (input[tmp] == ';')
    {
        to_add = init_token(T_SEPARATOR, T_SEMI,
                cut(input, &tmp, tmp + 1, len));
    }
    else if (input[tmp] == '&')
    {
        to_add = init_token(T_SEPARATOR, T_AND,
                cut(input, &tmp, tmp + 1, len));
    }
    add_token(lexer, to_add);
    return 1;
}

int is_command(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp == len)
        return 0;

    while (tmp < len && input[tmp] != ';' && input[tmp] != '&'
        && input[tmp] != '\n')
    {
        tmp++;
    }

    struct token *to_add = init_token(T_COMMAND, T_NONE,
        cut(input, index, tmp, len));
    add_token(lexer, to_add);
    to_add = NULL;

    if (input[tmp] == ';')
    {
        to_add = init_token(T_SEPARATOR, T_SEMI,
                cut(input, &tmp, tmp + 1, len));
    }
    else if (input[tmp] == '&')
    {
        to_add = init_token(T_SEPARATOR, T_AND,
                cut(input, &tmp, tmp + 1, len));
    }
    else if (input[tmp] == '\n')
    {
        to_add = init_token(T_SEPARATOR, T_NEWLINE,
                cut(input, &tmp, tmp + 1, len));
    }
    if (to_add)
    {
        tmp += 1;
        add_token(lexer, to_add);
    }
    *index = tmp;
    return 1;
}
