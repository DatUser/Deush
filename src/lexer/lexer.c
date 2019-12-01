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
#include "../quoting/header/quoting.h"

int LBRA = 0;
int LPAR = 0;
int IF = 0;
int DO = 0;

struct token_list *lexer;

/*!
 **  This functions checks if there is or not a separator, a creates a separator
 **  token if there is one.
 **  \param input : the string that may contain a separator
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 of the input contains a separator, 0 otherwse.
 */
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


/*!
 **  This functions cheks if the input contains a special word.
 **  \param input : the string that may contain a special word.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 0 if the input contains a valid special word, 0 otherwise.
 */
int is_legit(char *input, size_t *index, size_t len)
{
    if (*index < len - 4)
    {
        if (*index < len - 5 && input[*index + 2] != ' ')
        {
            return 1;
        }
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
        if (*index < len - 4 && input[*index + 2] != ' ')
        {
            return 1;
        }
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
        if (*index < len - 3 && input[*index + 2] != ' ')
        {
            return 1;
        }
        // FOR
        if (input[*index] == 'f' && input[*index + 1] == 'o'
            && input[*index + 2] == 'r')
        {
            return 0;
        }
    }
    if (*index < len - 1)
    {
        if (*index < len - 2 && input[*index + 2] != ' ')
        {
            return 1;
        }
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


/*!
 **  This function checks if there is an 'if' condition and creates the if
 **  tokens.
 **  \param input : the string that may contain an 'if' condition.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if there is an 'if' condition, 0 otherwise.
 */
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

    //is_command(input, index, len);
    is_WORD(input, index, len);
    IF = 1;
    return 1;
}


/*!
 **  This function checks if there is a 'then' conditionand creates the
 **  then tokens.
 **  \param input : the string that may contain a 'then' condition.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if there is a 'then' condition, 0 otherwise.
 */
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


/*!
 **  This function checks if there is an 'else' condition and creates the
 **  else tokens.
 **  \param input : the string that may contain an 'else' condition.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if there is an 'else' condition, 0 otherwise.
 */
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


/*!
 **  This function checks if there is an 'elif' condition and creates the elif
 **  tokens.
 **  \param input : the string that may contain an 'elif' condition.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if there is an 'elif' condition, 0 otherwise.
 */
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

    //is_command(input, index, len);
    is_WORD(input, index, len);
    return 1;
}


/*!
 **  This function checks if there is a 'fi' in the input and creates the
 **  fi token.
 **  \param input : the string that may contain a 'fi'.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 \return 1 if there is a 'fi', 0 otherwise.
 */
int is_fi(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (!((tmp < len - 2 && input[tmp] == 'f' && input[tmp + 1] == 'i'
        && input[tmp + 2] == ' ')
        || (tmp == len - 2 && input[tmp] == 'f' && input[tmp + 1] == 'i')))
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

/*!
 **  This functions checks if the input is a redirection.
 **  \param input : the string that may be a redirection.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input is a redirection, 0 otherwise.
 */
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


/*!
 **  This functions checks if the input contains a command.
 **  \param input : the string that may contain a command.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains a command, 0 otherwise.
 */
/*int is_command(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp == len)
        return 0;

    while (tmp < len && !which_separator(input[tmp]))
    {
        if (input[tmp] == ' ')
        {
            break;
        }
    }
    
    while ((tmp < len && input[tmp] != ';' && input[tmp] != '\n') ||
          (tmp < len && (SQUO || DQUO)))
    {
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
        else if (input[tmp] == '\r')
        {
            input[tmp] = '\n';
        }
        tmp++;
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
}*/


/*!
 **  This function checks if the input contains a comment.
 **  \param input : the string that may contain a comment
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains a comment, 0 otherwise.
 */
int is_comment(char *input, size_t *index, size_t len)
{

    if (*index == len)
        return 0;
    if (input[*index] == '#')
        return 1;
    return 0;
}


/*!
 **  This function checks if the input contains a 'while' condition and creates
 **  the while tokens.
 **  \param input : the string that may contain a 'while' condition.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains a 'while' condition, 0 otherwise.
 */
int is_while(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len - 4 || input[tmp] != 'w' || input[tmp + 1]  != 'h'
            || input[tmp + 2] != 'i' || input[tmp + 3] != 'l'
            || input[tmp + 4] != 'e')
    {
        return 0;
    }
    struct token *to_add = init_token(T_WHILE, T_WORD, cut(input, &tmp, tmp + 5,
                len));
    add_token(lexer, to_add);
    *index += 5;

    //is_command(input, index, len);
    is_WORD(input, index, len);
    return 1;
}


/*!
 **  This function checks if the input contains a 'do' condition and creates
 **  the do tokens.
 **  \param input : the string that may contain a 'do' condition.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains a 'do' condition, 0 otherwise.
 */
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


/*!
 **  This function checks if the input contains a 'done' condition and creates
 **  the done token.
 **  \param input : the string that may contain a 'done' condition.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains a 'done' condition, 0 otherwise.
 */
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

/*!
 **  This function checks if the input contains an 'until' condition and
 **  creates the until tokens.
 **  \param input : the string that may contain an 'until' condition.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains an 'until' condition, 0 otherwise.
 */
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


/*!
 **  This function checks if the input contains a funciton.
 **  \param input : the string that may contain a function.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains a function, 0 otherwise.
 */
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


/*!
 **  This function checks if the input contains a 'case' condition and creates
 **  the case token.
 **  \param input : the string that may contain a 'case' condition.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains a 'case' condition, 0 otherwise.
 */
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


/*!
 **  This function checks if the input contains an 'in' condition and creates
 **  the in tokens.
 **  \param input : the string that may contain an 'in' condition.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains an 'in' condition, 0 otherwise.
 */
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
    if (tmp != *index)
    {
        struct token *to_add = init_token(T_WORD, T_NONE,
                    cut(input, index, tmp, len));
        add_token(lexer, to_add);
        remove_white_space(input, &tmp, len);
        *index = tmp;
    }
    *index = tmp;
    return 1;
}

int which_separator(char to_check)
{
    switch (to_check)
    case '>':
    case '<':
    case '&':
    case '|':
    case '\n':
    case ';':
    case '(':
    case ')':
    case '{':
    case '}':
        return 1;
    return 0;
}


/*!
 **  This function handles the parsing for a shopt.
 **  \param input : the string that may contain a shopt
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains a shopt, 0 otherwise.
 */
int handle_builtin(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    int expand = 1;
    while (tmp < len && input[tmp] != ';' && input[tmp] != '\n'
            && !which_separator(input[tmp]))
    {
        if (input[tmp] == ' ')
        {
            struct token *to_add = init_token(T_WORD, T_NONE,
                    cut(input, index, tmp, len));
            if (input[*index] == '$' && expand)
            {
                to_add->secondary_type = T_EXPAND;
            }
            add_token(lexer, to_add);
            remove_white_space(input, &tmp, len);
            *index = tmp;
            expand = 1;
        }
        else if (input[tmp] == '\'')
        {
            len = unquote_squotes(input, &tmp, len);
            expand = 0;
        }
        else if (input[tmp] == '\"')
        {
            len = unquote_dquotes(input, &tmp, len);
            *index = tmp;
        }
        else
        {
            tmp++;
        }
    }
    if (tmp != *index)
    {
        struct token *to_add = init_token(T_WORD, T_NONE,
                cut(input, index, tmp, len));
        if (input[*index] == '$' && expand)
        {
            to_add->secondary_type = T_EXPAND;
        }
        add_token(lexer, to_add);
        remove_white_space(input, &tmp, len);
        *index = tmp;
    }
    *index = tmp;
    return 1;
}


int is_variable(char *input, size_t *index, size_t len, size_t tmp)
{
    if (input[tmp + 1] == ' ')
    {
        return 0;
    }
    char *name_var = cut(input, index, tmp, len);
    char *equals = cut(input, &tmp, tmp + 1, len);
    *index = tmp + 1;
    tmp = *index;
    while (tmp < len && input[tmp] != ' ')
    {
        tmp++;
    }
    char *var_value = cut(input, index, tmp, len);
    struct token *name = init_token(T_VARNAME, T_NONE, name_var);
    struct token *equal_op = init_token(T_OPERATOR, T_EQUAL, equals);
    struct token *value = init_token(T_WORD, T_NONE, var_value);
    add_token(lexer, name);
    add_token(lexer, equal_op);
    add_token(lexer, value);
    *index = tmp;
    return 1;
}

/*!
 **  This function checks if the input contiansa word and creates the
 **  word token.
 **  \param input : the string that may contain a word.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains a word, 0 otherwise.
 */
int is_WORD(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len || ((input[tmp] >= 48 && input[tmp] <= 57)
                || isspace(input[tmp])))
    {
        return 0;
    }
    //tmp += 1;

    if (input[tmp] == '$')
    {
        while (tmp < len && input[tmp] != ' ')
        {
            tmp++;
        }
        struct token *to_add = init_token(T_WORD, T_EXPAND,
            cut(input, index, tmp, len));
        add_token(lexer, to_add);
        *index = tmp;
        return 1;
    }
    else if (input[tmp] == '\'')
    {
        *index = tmp + 1;
    }
    tmp += 1;

    while (tmp < len && !(isblank(input[tmp])) && !which_separator(input[tmp]))
    {
        if (input[tmp] >= 1 && input[tmp] <= 6)
            break;
        if (input[tmp] >= 14 && input[tmp] <= 31)
            break;
        if (input[tmp] == 127)
            break;
        if (input[tmp] == '=')
        {
            if (is_variable(input, index, len, tmp))
            {
                return 1;
            }
        }
        if (input[tmp] == '\'')
        {
            struct token *to_add = init_token(T_WORD, T_NONE, cut(input,
                index, tmp, len));
            add_token(lexer, to_add);
            *index = tmp + 1;
            return 1;
        }
        tmp++;
    }
    size_t tmp3 = tmp;
    remove_white_space(input, &tmp3, len);
    char *string_to_add = cut(input, index, tmp, len);
    if (strlen(string_to_add) > 2 && string_to_add[0] == '.'
        && string_to_add[1] == '/')
    {
        struct token *to_add = init_token(T_SCRIPT, T_WORD, string_to_add);
        add_token(lexer, to_add);
        *index = tmp;
        handle_builtin(input, index, len);
        return 1;
    }
    if (is_builtin(string_to_add)) // REPLACE W/ BUILTIN
    {
        struct token *to_add = init_token(T_BUILTIN, T_WORD, string_to_add);
        add_token(lexer, to_add);
        *index = tmp;
        handle_builtin(input, index, len);
        return 1;
    }
    if ((tmp3 != len && input[tmp3] != '&' && input[tmp3] != ';' &&
                input[tmp3] != '|'
                && input[tmp3] != '\n') && is_legit(input, &tmp3, len))
    {
        if (input[tmp3] == '(')
        {
            struct token *to_add = init_token(T_FUNCTION_NAME, T_COMMAND,
                    string_to_add);
            add_token(lexer, to_add);
            *index = tmp;
            return 1;
        }
        struct token *to_add = init_token(T_COMMAND, T_WORD, string_to_add);
        add_token(lexer, to_add);
        *index = tmp;
        handle_builtin(input, index, len);
        //free(string_to_add);
        return 1;
    }
    struct token *to_add = init_token(T_WORD, T_NONE, string_to_add);
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


/*!
 **  This functions checks if the input contains an 'esac' condition and
 **  creates the esac token.
 **  \param input : the string that may contain an 'esac' condition.
 **  \param index : the current index in the string.
 **  \param len : the length of the input string.
 **  \return 1 if the input contains an 'esac' condition, 0 otherwise.
 */
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
        nb[0] = '0';
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
    if (op && op[0] == '>')
    {
        nb[0] = '1';
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
    /*
    while (!isblank(input[tmp]) && tmp < len)
    {
        tmp += 1;
    }
    char *word = cut(input, index, tmp, len);
    struct token *w = init_token(T_WORD, T_NONE, word);
    add_token(lexer, w);
    *index = tmp;
*/ 
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
