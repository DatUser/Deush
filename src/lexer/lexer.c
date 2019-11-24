/*!
 **  \file lexer.c
 **  \brief This file contains all the functions related to the lexer.
 **  \author 42sh Group
 */

#include "../include/global.h"
#include "header/lexer.h"
#include "header/token.h"
#include "../prompt/header/prompt.h"
#include "../auxiliary/header/auxiliary.h"
#include "../include/include.h"

int LBRA = 0;
int LPAR = 0;
int IF = 0;
int DO = 0;

struct token_list *lexer;

int is_separator(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len)
    {
        return 0;
    }
    if (input[*index] == '\n')
    {
        struct token *to_add = init_token(T_SEPARATOR, T_NEWLINE,
                cut(input, index, *index + 1, len));
        add_token(lexer, to_add);
        *index += 1;
    }
    else if (input[*index] == '{')
    {
        struct token *to_add = init_token(T_SEPARATOR, T_LBRACE,
                cut(input, index, *index + 1, len));
        add_token(lexer, to_add);
        *index += 1;
        LBRA = 1;
    }
    else if (input[*index] == '}')
    {
        struct token *to_add = init_token(T_SEPARATOR, T_RBRACE,
                cut(input, index, *index + 1, len));
        add_token(lexer, to_add);
        *index += 1;
        LBRA = 0;
    }
    else if (input[*index] == '&')
    {
        if (*index < len - 1 && input[*index + 1] == '&')
        {
            struct token *to_add = init_token(T_SEPARATOR, T_ANDIF,
                    cut(input, index, *index + 2, len));
            add_token(lexer, to_add);
            *index += 2;
        }
        else
        {
            struct token *to_add = init_token(T_SEPARATOR, T_AND,
                    cut(input, index, *index + 1, len));
            add_token(lexer, to_add);
            *index += 1;
        }
    }
    else if (input[*index] == '|')
    {
        if (*index < len - 1 && input[*index + 1] == '|')
        {
            struct token *to_add = init_token(T_SEPARATOR, T_ORIF,
                    cut(input, index, *index + 2, len));
            add_token(lexer, to_add);
            *index += 2;
        }
        else
        {
            struct token *to_add = init_token(T_SEPARATOR, T_PIPE,
                    cut(input, index, *index + 1, len));
            add_token(lexer, to_add);
            *index += 1;
        }
    }
    else if (input[*index] == '(')
    {
        struct token *to_add = init_token(T_SEPARATOR, T_LPAR,
                cut(input, index, *index + 1, len));
        add_token(lexer, to_add);
        *index += 1;
        LPAR = 1;
    }
    else if (input[*index] == ')')
    {
        struct token *to_add = init_token(T_SEPARATOR, T_RPAR,
                cut(input, index, *index + 1, len));
        add_token(lexer, to_add);
        *index += 1;
        LPAR = 0;
    }
    else if (input[*index] == ';')
    {
        if (*index < len - 1 && input[*index + 1] == ';')
        {
            struct token *to_add = init_token(T_SEPARATOR, T_DSEMI,
                    cut(input, index, *index + 2, len));
            add_token(lexer, to_add);
            *index += 2;
        }
        else
        {
            struct token *to_add = init_token(T_SEPARATOR, T_SEMI,
                    cut(input, index, *index + 1, len));
            add_token(lexer, to_add);
            *index += 1;
        }
    }
    else
    {
        return 0;
    }
    return 1;
}

int is_legit(char *input, size_t *index, size_t len)
{
    if (*index < len - 4)
    {
        // WHILE
        if (input[*index] == 'w'
                && input[*index + 1] == 'h' && input[*index + 2] == 'i'
                && input[*index + 3] == 'l' && input[*index + 4] == 'e')
        {
            return 0;
        }
        // UNTIL
        if (input[*index] == 'u'
                && input[*index + 1] == 'n' && input[*index + 2] == 't'
                && input[*index + 3] == 'i' && input[*index + 4] == 'l')
        {
            return 0;
        }
    }
    if (*index < len - 3)
    {
        // CASE
        if (input[*index] == 'c' && input[*index + 1] == 'a'
                && input[*index + 2] == 's' && input[*index + 3] == 'e')
        {
            return 0;
        }
        // ESAC
        if (input[*index] == 'e' && input[*index + 1] == 's'
                && input[*index + 2] == 'a' && input[*index + 3] == 'c')
        {
            return 0;
        }
        // ELSE
        if (input[*index] == 'e' && input[*index + 1] == 'l'
                && input[*index + 2] == 's' && input[*index + 3] == 'e')
        {
            return 0;
        }
        // ELIF
        if (input[*index] == 'e' && input[*index + 1] == 'l'
                && input[*index + 2] == 'i' && input[*index + 3] == 'f')
        {
            return 0;
        }
        // THEN
        if (input[*index] == 't' && input[*index + 1] == 'h'
                && input[*index + 2] == 'e' && input[*index + 3] == 'n')
        {
            return 0;
        }
        // DONE
        if (input[*index] == 'd' && input[*index + 1] == 'o'
                && input[*index + 2] == 'n' && input[*index + 3] == 'e')
        {
            return 0;
        }
    }
    if (*index < len - 2)
    {
        // FOR
        if (input[*index] == 'f' && input[*index + 1] == 'o'
                && input[*index + 2] == 'r')
        {
            return 0;
        }
    }
    if (*index < len - 1)
    {
        // IN
        if (input[*index] == 'i' && input[*index + 1] == 'n')
        {
            return 0;
        }
        // FI
        if (input[*index] == 'f' && input[*index + 1] == 'i')
        {
            return 0;
        }
        // DO
        if (input[*index] == 'd' && input[*index + 1] == 'o')
        {
            return 0;
        }
        // IF
        if (input[*index] == 'i' && input[*index + 1] == 'f')
        {
            return 0;
        }
    }
    return 1;
}

int is_if(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    if (tmp >= len - 1 || input[tmp] != 'i' || input[tmp + 1] != 'f')
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
    IF = 1;
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

    is_separator(input, index, len);
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

    is_separator(input, index, len);
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

    remove_white_space(input, index, len);
    IF = 0;
    is_separator(input, index, len);
    return 1;
}

int is_redirection(char *input, size_t index, size_t len)
{
    remove_white_space(input, &index, len);
    while (index < len && input[index] != ' ')
    {
        if (input[index] == '<')
            return 1;
        if (input[index] == '>')
            return 1;
        index++;
    }
    return 0;
}

int is_command(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp == len)
        return 0;

    while (tmp < len && input[tmp] != ';' && input[tmp] != '\n')
    {
        tmp++;
        if (input[tmp] == ' ')
        {
            size_t tmp3 = tmp;
            if (is_separator(input, &tmp3, len))
            {
                struct token *to_add = init_token(T_COMMAND, T_NONE,
                        cut(input, index, tmp, len));
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
                *index = tmp3;
                if (input[tmp] == ';')
                {
                    tmp = tmp3;
                    break;
                }
            }
            else if (is_redirection(input, tmp3, len))
            {
                break;
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
    is_separator(input, index, len);
    return 1;
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
            || input[tmp + 2] != 'i' || input[tmp + 3] != 'l'
            || input[tmp + 4] !=
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
                || input[*index + 1] != 'o'))
    {
        return 0;
    }
    struct token *to_add = init_token(T_DO, T_WORD, cut(input, index,
                *index + 2, len));
    add_token(lexer, to_add);
    *index += 2;
    DO = 1;
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
    DO = 0;
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

int is_function(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len - 7 || input[*index] != 'f' || input[*index + 1] != 'u'
            || input[*index + 2] != 'n' || input[*index + 3] != 'c'
            || input[*index + 4] != 't' || input[*index + 5] != 'i'
            || input[*index + 6] != 'o' || input[*index + 7] != 'n')
    {
        return 0;
    }
    struct token *to_add = init_token(T_FUNCTION, T_WORD, cut(input, index,
                *index + 8, len));
    add_token(lexer, to_add);
    *index += 8;
    return 1;
}

int is_case(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len - 3 || input[*index] != 'c' || input[*index + 1] != 'a'
            || input[*index + 2] != 's' || input[*index + 3] != 'e')
    {
        return 0;
    }
    struct token *to_add = init_token(T_CASE, T_WORD, cut(input, index, *index
                + 4, len));
    add_token(lexer, to_add);
    *index += 4;
    remove_white_space(input, index, len);
    is_WORD(input, index, len);
    return 1;
}

int is_in(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len - 1 || input[*index] != 'i' || input[*index + 1] != 'n')
    {
        return 0;
    }
    struct token *to_add = init_token(T_IN, T_WORD, cut(input, index, *index
                + 2, len));
    add_token(lexer, to_add);
    *index += 2;
    remove_white_space(input, index, len);
    size_t tmp = *index;
    while (tmp < len && input[tmp] != ';' && input[tmp] != '|'
            && input[tmp] != ')')
    {
        if (input[tmp] == ' ')
        {
            struct token *to_add = init_token(T_WORD, T_NONE,
                    cut(input, index, tmp, len));
            add_token(lexer, to_add);
            remove_white_space(input, &tmp, len);
            *index = tmp;
        }
        else
        {
            tmp++;
        }
    }
    *index = tmp;
    return 1;
}

int handle_shopt(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    while (tmp < len && input[tmp] != ';' && input[tmp] != '\n')
    {
        if (input[tmp] == ' ')
        {
            struct token *to_add = init_token(T_WORD, T_NONE,
                cut(input, index, tmp, len));
            add_token(lexer, to_add);
            remove_white_space(input, &tmp, len);
            *index = tmp;
        }
        else
        {
            tmp++;
        }
    }
    if (tmp == len && tmp != *index)
    {
        struct token *to_add = init_token(T_WORD, T_NONE,
                cut(input, index, tmp, len));
            add_token(lexer, to_add);
            remove_white_space(input, &tmp, len);
            *index = tmp;
    }
    return 1;
}

int is_WORD(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len || ((input[tmp] >= 48 && input[tmp] <= 57)
                || isspace(input[tmp])))
    {
        return 0;
    }
    tmp += 1;

    while (tmp < len && !(isblank(input[tmp])) && input[tmp] != ';'
            && input[tmp] != '&')
    {
        if (input[tmp] >= 1 && input[tmp] <= 6)
            break;
        if (input[tmp] >= 14 && input[tmp] <= 31)
            break;
        if (input[tmp] == 127)
            break;

        tmp++;
    }
    size_t tmp3 = tmp;
    remove_white_space(input, &tmp3, len);
    char *string_to_add = cut(input, index, tmp, len);
    if (strcmp(string_to_add, "shopt") == 0) // REPLACE W/ BUILTIN
    {
        struct token *to_add = init_token(T_BUILTIN, T_WORD, string_to_add);
        add_token(lexer, to_add);
        *index = tmp;
        handle_shopt(input, index, len);
        return 1;
    }
    if ((tmp3 != len && input[tmp3] != '&' && input[tmp3] != ';' &&
                input[tmp3] != '|' && input[tmp3] != ')' && input[tmp3] != '('
                && input[tmp3] != '\n') && is_legit(input, &tmp3, len))
    {
        if (input[tmp3] == '{')
        {
            struct token *to_add = init_token(T_FUNCTION_NAME, T_COMMAND,
            string_to_add);
            add_token(lexer, to_add);
            *index = tmp;
            return 1;
        }
        free(string_to_add);
        return 0;
    }
    struct token *to_add = init_token(T_WORD, T_COMMAND, string_to_add);
    add_token(lexer, to_add);
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



int is_esac(char *input, size_t *index, size_t len)
{
    if (*index >= len - 3 || input[*index] != 'e' || input[*index + 1] != 's'
            || input[*index + 2] != 'a' || input[*index + 3] != 'c')
    {
        return 0;
    }
    struct token *to_add = init_token(T_ESAC, T_WORD, cut(input, index, *index
                + 4, len));
    add_token(lexer, to_add);

    *index += 4;
    return 1;
}

/*!
 **  This funcitons add a token operator related to the redirection in the
 **  token list.
 **  \param input : The string that will be the value of the token.
 **  \return 1 if the token could be createad and added, 0 otherwise.
 */
int add_redirect(char *input, char *nb)
{
    if (!input)
    {
        return 0;
    }
    if (strcmp(input, ">") == 0)
    {
        size_t size =  strlen(input) + strlen(nb) + 1;
        char *s = malloc(sizeof(char) * size);
        if (s == NULL)
        {
            return 0;
        }
        s = strcpy(s, nb);
        s = strcat(s, input);
        struct token *t = init_token(T_GREATER, T_NONE, s);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<") == 0)
    {
        size_t size =  strlen(input) + strlen(nb) + 1;
        char *s = malloc(sizeof(char) * size);
        if (s == NULL)
        {
            return 0;
        }
        s = strcpy(s, nb);
        s = strcat(s, input);
        struct token *t = init_token(T_LESS, T_NONE, s);
        add_token(lexer, t);
    }
    else if (strcmp(input, ">>") == 0)
    {
        size_t size =  strlen(input) + strlen(nb) + 1;
        char *s = malloc(sizeof(char) * size);
        if (s == NULL)
        {
            return 0;
        }
        s = strcpy(s, nb);
        s = strcat(s, input);
        struct token *t = init_token(T_RGREAT, T_NONE, s);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<<") == 0)
    {
        size_t size =  strlen(input) + strlen(nb) + 1;
        char *s = malloc(sizeof(char) * size);
        if (s == NULL)
        {
            return 0;
        }
        s = strcpy(s, nb);
        s = strcat(s, input);
        struct token *t = init_token(T_RLESS, T_HEREDOC, s);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<<-") == 0)
    {
        size_t size =  strlen(input) + strlen(nb) + 1;
        char *s = malloc(sizeof(char) * size);
        if (s == NULL)
        {
            return 0;
        }
        s = strcpy(s, nb);
        s = strcat(s, input);
        struct token *t = init_token(T_RLESSDASH, T_HEREDOC, s);
        add_token(lexer, t);
    }
    else if (strcmp(input, ">&") == 0)
    {
        size_t size =  strlen(input) + strlen(nb) + 1;
        char *s = malloc(sizeof(char) * size);
        if (s == NULL)
        {
            return 0;
        }
        s = strcpy(s, nb);
        s = strcat(s, input);
        struct token *t = init_token(T_GREATAND, T_NONE, s);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<&") == 0)
    {
        size_t size =  strlen(input) + strlen(nb) + 1;
        char *s = malloc(sizeof(char) * size);
        if (s == NULL)
        {
            return 0;
        }
        s = strcpy(s, nb);
        s = strcat(s, input);
        struct token *t = init_token(T_LESSAND, T_NONE, s);
        add_token(lexer, t);
    }
    else if (strcmp(input, ">|") == 0)
    {
        size_t size =  strlen(input) + strlen(nb) + 1;
        char *s = malloc(sizeof(char) * size);
        if (s == NULL)
        {
            return 0;
        }
        s = strcpy(s, nb);
        s = strcat(s, input);
        struct token *t = init_token(T_CLOBBER, T_NONE, s);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<>") == 0)
    {
        size_t size =  strlen(input) + strlen(nb) + 1;
        char *s = malloc(sizeof(char) * size);
        if (s == NULL)
        {
            return 0;
        }
        s = strcpy(s, nb);
        s = strcat(s, input);
        struct token *t = init_token(T_LESSGREAT, T_NONE, s);
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
    char *nb;
    size_t tmp2 = 0;
    char *op = NULL;
    remove_white_space(input, &tmp, len);
    *index = tmp;
    if (input[tmp] >= 48 && input[tmp] <= 57)
    {
        while (input[tmp] >= 48 && input[tmp] <= 57 && tmp < len)
        {
            tmp += 1;
        }

        nb = cut(input, index, tmp, len);
        *index = tmp;
    }
    else
    {
        nb = calloc(sizeof(char), 2);
        if (nb == NULL)
        {
            return 0;
        }
        nb[0] = '1';
        tmp2 = tmp;
    }


    while (!isblank(input[tmp]) && tmp < len)
    {
        tmp += 1;
    }
    if (tmp2 != 0)
    {
        op = cut(input, &tmp2, tmp, len);
    }
    else
    {
        op = cut(input, index, tmp, len);
    }
    if (!add_redirect(op, nb))
    {
        free(op);
        free(nb);
        return 0;
    }
    free(op);
    free(nb);

    remove_white_space(input, &tmp, len);
    *index = tmp;

    while (!isblank(input[tmp]) && tmp < len)
    {
        tmp += 1;
    }
    char *word = cut(input, index, tmp, len);
    struct token *w = init_token(T_WORD, T_NONE, word);
    add_token(lexer, w);
    *index = tmp;

    return 1;
}


/*!
 **  This function add the rule_for nodes to the token list.
 **  \param input : the string that contains the information.
 **  \param index : the current index in the input string.
 **  \param len : the length of the input string.
 **  \return 1 if the nodes could be added to the token list, 0 otherwise.
 */
int is_for(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    remove_white_space(input, &tmp, len);
    *index = tmp;

    if (tmp >= len - 2 || input[tmp] != 'f' || input[tmp + 1] != 'o'
            || input[tmp + 2] != 'r')
    {
        return 0;
    }
    tmp += 3;
    char *f = cut(input, index, tmp, len);
    struct token *token_for = init_token(T_FOR, T_NONE, f);
    add_token(lexer, token_for);

    *index = tmp;

    is_WORD(input, &tmp, len);

    remove_white_space(input, &tmp, len);
    *index = tmp;

    int i = is_separator(input, &tmp, len);

    if (i == 0)
    {
        is_in(input, &tmp, len);
        /*
           while (input[tmp] == '\n' && tmp < len)
           {
           add_newline();
           tmp += 1;
           }
           remove_white_space(input, &tmp, len);
         *index = tmp;

         if (tmp >= len - 1 || input[tmp] != 'i' || input[tmp + 1] != 'n')
         {
         return 0;
         }
         tmp += 2;
         char *in_w = cut(input, index, tmp, len);
         struct token *in_token = init_token(T_IN, T_NONE, in_w);
         add_token(lexer, in_token);

         remove_white_space(input, &tmp, len);
         is_WORD(input, &tmp, len);
         *index = tmp;

         int j = is_separator(input, &tmp, len);
         if (j == 0)
         {
         if (input[tmp] == '\n')
         {
         add_newline();
         }
         }*/
    }

    *index = tmp;
    while (input[tmp] == '\n')
    {
        add_newline();
        tmp += 1;
    }

    *index = tmp;
    is_do(input, &tmp, len);
    is_done(input, &tmp, len);
    *index = tmp;

    return 1;
}

/*int main(void)
  {
  lexer = init_token_list();
  char *s = "for kkk \n in tmp \n\n do azer done";
  size_t i = 0;
  size_t len = strlen(s);

  printf("%d\n", is_for(s, &i, len));
  token_printer(lexer);
  }*/
