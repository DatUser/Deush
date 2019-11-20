/*!
 **  \file lexer.c
 **  \brief This file contains all the functions related to the lexer.
 **  \author 42sh Group
 */

#include "header/lexer.h"
#include "header/token.h"
#include "../auxiliary/header/auxiliary.h"
#include "../include/include.h"

struct token_list *lexer;

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


/*!
**  This function adds a bang token to the token list if there is one.
**  \param input : the string that may contain a bang character.
**  \param index : the current index in the string.
**  \param len : the length of the input string.
**  \return 1 if the token could be added to the token list, -1 otherwise.
*/
int add_bang(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    remove_white_space(input, &tmp, len);
    if (input[tmp] == '!')
    {
        char *s = cut(input, index, tmp + 1, len);
        struct token *bang = init_token(T_BANG, T_NONE, s);
        if (bang == NULL)
        {
            return -1; //error
        }

        lexer = add_token(lexer, bang);
        tmp += 1;
        *index = tmp;
        return 1;
    }

    return 0;
}


/*!
**  This function adds a pipe token to the token list if there is one.
**  \param p : the string containing the pipe character onlu.
**  \param index : the actual index in the string that the input string come
**   from.
**  \return 1 if the token could be added to the token list, -1 otherwise.
*/
int add_pipe(char *p, size_t *index)
{
    size_t tmp = *index;
    struct token *pipe = init_token(T_PIPE, T_NONE, p);
    if (pipe == NULL)
    {
        return -1;
    }

    lexer = add_token(lexer, pipe);
    *index = tmp;
    return 1;
}


/*!
**  This function adds a newline token to the token list.
*/
void add_newline(void)
{
    char *s = malloc(sizeof(char) * 2);
    if (s == NULL)
    {
        return;
    }

    s = "\n";
    struct token *newline = init_token(T_NEWLINE, T_NONE, s);
    add_token(lexer, newline);
}


/*!
**  This function adds the pipeline tokens to the token list.
**  \param input : the string to be parsed.
**  \param index : the current index in the input string.
**  \param len : the length of the input string.
**  \return 1 if there is no errors, 0 otherwise.
*/
int pipelines(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    if (add_bang(input, &tmp, len) == -1)
    {
        return 0;
    }

    remove_white_space(input, &tmp, len);
    *index = tmp;

    while (tmp < len)
    {
        while (input[tmp] != '|' && tmp < len)
        {
            tmp += 1;
        }

        char *c = cut(input, index, tmp, len);
        struct token *command = init_token(T_COMMAND, T_NONE, c);
        if (command == NULL)
        {
            return 0;
        }

        lexer = add_token(lexer, command);
        *index = tmp;
        tmp += 1;

        char *p = cut(input, index, tmp, len);
        if (strcmp(p, "|") == 0)
        {
            if (add_pipe(p, &tmp) == -1)
            {
                return 0;
            }
        }

        remove_white_space(input, &tmp, len);
        *index = tmp;

        if (input[tmp] == '\n')
        {
            return 1;
        }
    }

    add_newline();

    return 1;
}


/*!
**  This funcitons add a token operator related to the redirection in the
**  token list.
**  \param input : The string that will be the value of the token.
**  \return 1 if the token could be createad and added, 0 otherwise.
*/
int add_redirect(char *input)
{
    if (strcmp(input, ">") == 0)
    {
        struct token *t = init_token(T_GREATER, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<") == 0)
    {
        struct token *t = init_token(T_LESS, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, ">>") == 0)
    {
        struct token *t = init_token(T_RGREAT, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<<") == 0)
    {
        struct token *t = init_token(T_RLESS, T_HEREDOC, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<<-") == 0)
    {
        struct token *t = init_token(T_RLESSDASH, T_HEREDOC, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, ">&") == 0)
    {
        struct token *t = init_token(T_GREATAND, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<&") == 0)
    {
        struct token *t = init_token(T_LESSAND, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, ">|") == 0)
    {
        struct token *t = init_token(T_CLOBBER, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<>") == 0)
    {
        struct token *t = init_token(T_LESSGREAT, T_NONE, input);
        add_token(lexer, t);
    }
    else
    {
        return 0;
    }

    return 1;
}

/*!
**  This function add the redirection nodes to the token list.
**  \param input : the string that contains the information.
**  \param index : the current index in the input string.
**  \param len : the length of the input string.
*/
int redirection(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    remove_white_space(input, &tmp, len);
    *index = tmp;

    while (tmp < len)
    {
        while (input[tmp] != ' ')
        {
            tmp += 1;
        }

        char *nb = cut(input, index, tmp, len);
        struct token *ionb = init_token(T_WORD, T_NONE, nb);
        if (ionb == NULL)
        {
            return 0;
        }

        add_token(lexer, ionb);
        *index = tmp;

        remove_white_space(input, &tmp, len);
        *index = tmp;

        while (input[tmp] != ' ')
        {
            tmp += 1;
        }

        char *op = cut(input, index, tmp, len);
        add_redirect(op);

        remove_white_space(input, &tmp, len);
        *index = tmp;

        while (tmp < len)
        {
            tmp += 1;
        }

        char *word = cut(input, index, tmp, len);
        struct token *w = init_token(T_WORD, T_NONE, word);
        add_token(lexer, w);
        *index = tmp;
    }

    add_newline();
    return 1;
}
