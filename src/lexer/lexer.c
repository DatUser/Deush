/*!
 **  \file lexer.c
 **  \brief This file contains all the functions related to the lexer.
 **  \author 42sh Group
 */

#include "../include/include.h"
#include "header/token.h"
#include "header/lexer.h"
#include "../auxiliary/header/auxiliary.h"

struct token_list *lexer = NULL;

/*!
**  This function removes any whitespace before a block to text.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
*/
void remove_white_space(char *input, size_t *index, size_t len)
{
    while (*index < len && input[*index] != '\0' && isspace(input[*index]))
    {
        *index += 1;
    }
}

/*!
**  This function checks the io_file grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the io_file grammar is correct, 0 otherwise.
*/
int is_io_file(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    if (input[tmp] != '<' &&
        (input[tmp] != '<' || input[tmp + 1] != '&') &&
        input[tmp] != '>' &&
        (input[tmp] != '>' || input[tmp + 1] != '&') &&
        (input[tmp] != '>' || input[tmp + 1] != '>') &&
        (input[tmp] != '>' || input[tmp + 1] != '|'))
    {
        return 0;
    }

    while (input[tmp] != ' ')
    {
        tmp += 1; //ADVANCE TO FILENAME
    }

    tmp += 1; //REMOVE WHITE SPACE

    if (is_WORD(input, &tmp, len))
    {
        *index = tmp;
        return 1;
    }

    return 0;
}


/*!
**  This function checks the io_here grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the io_here grammar is correct, 0 otherwise.
*/
int is_io_here(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    if (tmp >= len - 1 || ((input[tmp] != '<' || input[tmp + 1] != '<') &&
        (input[tmp] != '<' || input[tmp + 1] != '<' || input[tmp + 2] != '-')))
    {
        return 0;
    }

    while (tmp < len && input[tmp] != ' ')
    {
        tmp += 1;
    }
    tmp += 1;

    if (is_WORD(input, &tmp, len))
    {
        *index = tmp;
        return 1;
    }
    return 0;
}


/*!
**  This function checks the IO_NUMBER grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the IO_NUMBER grammar is correct, 0 otherwise.
*/
int is_IO_NUMBER(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    // RM WHITESPACES

    while (tmp < len && (input[tmp] >= '0' && input[tmp] <= '9'))
    {
        tmp += 1;
    }

    if (tmp == *index)
    {
        return 0;
    }

    if (tmp < len || input[tmp] == '>')
    {
        *index = tmp + 1;
    }
    else if (tmp < len || input[tmp] == '<')
    {
        *index = tmp + 1;
    }
    else
    {
        return 0;
    }

    return 1;
}

/*!
**  This function checks the io_redirect grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the io_redirect grammar is correct, 0 otherwise.
*/
int is_io_redirect(char *input, size_t *index, size_t len)
{
    if (is_io_file(input, index, len))
    {
        return 1;
    }
    else if (is_IO_NUMBER(input, index, len))
    {
        if (is_io_file(input, index, len))
        {
            return 1;
        }
        if (is_io_here(input, index, len))
        {
            return 1;
        }
    }
    else if (is_io_here(input, index, len))
    {
        return 1;
    }
    return 0;
}

/*!
 **  This function checks if the input has an equal character.
 **  \param input : the string to be checked.
 **  \param index : the current index in the input string.
 **  \return 1 if the input has an equal character, 0 otherwise.
 */
int has_equal_character(char *input, size_t *index)
{
    size_t len = strlen(input);
    size_t i = *index;
    while (i < len)
    {
        if (input[i] == '=')
        {
            return 1;
        }
        i++;
    }
    return 0;
}


/*!
 **  This function checks if the input is an assignment.
 **  \param input : the string to be checked.
 **  \param index : the current index in the string input.
 **  \param len : the actual length of the input string.
 **  \return 1 if the input is an assignment, 0 otherwise.
 */
int is_ASSIGNMENT_WORD(char *input, size_t index, size_t len)
{
    char *s = strdup(input);
    s = s + index;

    if ((has_equal_character(s, &index)) == 0)
    {
        return 0;
    }

    if (s[0] == '=')
    {
        return 0;
    }

    char *name = strtok(s, "=");
    if (is_WORD(name, 0, len) == 0)
    {
        return 0;
    }

    free(s);
    return 1;
}


/*!
**  This function checks the cmd_prefix grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the cmd_prefix grammar is correct, 0 otherwise.
*/
int is_cmd_prefix(char *input, size_t *index, size_t len)
{
    if (!is_io_redirect(input, index, len) && !is_ASSIGNMENT_WORD(input,
        *index, len))
    {
        return 0;
    }

    while (is_io_redirect(input, index, len) || is_ASSIGNMENT_WORD(input,
           *index, len));

    return 1;
}


/*!
**  This function checks the cmd_name grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 is the cmd_grammar is correct, 0 otherwise.
*/
int is_cmd_name(char *input, size_t *index, size_t len)
{
    return is_WORD(input, index, len);
}

/*!
**  This function checks the cmd_word grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the cmd_word grammar is correct, 0 otherwise.
*/
int is_cmd_word(char *input, size_t *index, size_t len)
{
    return is_WORD(input, index, len); // APPLY RULE 7B
}

/*!
**  This function checks the cmd_suffix grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the cmd_suffix grammar is correct, 0 otherwise.
*/
int is_cmd_suffix(char *input, size_t *index, size_t len)
{
    if (!is_io_redirect(input, index, len) || !is_WORD(input, index, len))
    {
        return 0;
    }

    while (is_io_redirect(input, index, len) || is_WORD(input, index, len));

    return 1;
}


/*!
**  This function checks the simple_command grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the simple_command grammar is correct, 0 otherwise.
*/
int is_simple_command(char *input, size_t *index, size_t len)
{
    if (is_cmd_prefix(input, index, len))
    {
        if (is_cmd_word(input, index, len))
        {
            if (is_cmd_suffix(input, index, len))
            {
                return 1;
            }
            return 1;
        }
        return 1;
    }
    else if (is_cmd_name(input, index, len))
    {
        if (is_cmd_suffix(input, index, len))
        {
            return 1;
        }
        return 1;
    }
    return 0;
}


/*!
**  This function checks the compound_command grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the compound_command grammar is correct, 0 otherwise.
*/
int is_compound_command(char *input, size_t *index, size_t len)
{
    //if (is_brace_group(input, index))
    //else if (is_subshell(input, index))
    //else if (is_for_clause(input, index))
    /* else */ if (is_if_clause(input, index, len))
    {
        return 1;
    }
    //else if (is_while_clause(input, index))
    //else if (is_until_clause(input, index))
    return 0;
}


/*!
**  This function checks ithe redirect_list grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
*  \return 1 if the redirect_list grammar is correct, 0 otherwise.
*/
int is_redirect_list(char *input, size_t *index, size_t len)
{
    if (!is_io_redirect(input, index, len))
    {
        return 0;
    }

    while (is_io_redirect(input, index, len));

    return 1;
}

/*!
**  This function checks the fname grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the fname grammar is correct, 0 otherwise.
*/
int is_fname(char *input, size_t *index, size_t len)
{
    return is_WORD(input, index, len);
}


/*!
**  This function checks the function_body grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the function_body grammar is correct, 0 otherwise.
*/
int is_function_body(char *input, size_t *index, size_t len)
{
    if (is_compound_command(input, index, len)) // APPLY RULE 9
    {
        if (is_redirect_list(input, index, len)) //APPLY RULE 9
        {
            return 1;
        }
        return 1;
    }
    return 0;
}


/*!
**  This function checks the function_definition grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the function_definition grammar is correct, 0 otherwise.
*/
int is_function_definition(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    if (!is_fname(input, &tmp, len))
    {
        return 0;
    }

    if (tmp >= len || input[tmp] != '(')
    {
        return 0;
    }
    tmp += 1;

    // RM WHITESPACE

    if (tmp >= len || input[tmp] != ')')
    {
        return 0;
    }
    tmp += 1;

    //RM WHITESPACE

    if (!is_linebreak(input, &tmp, len))
    {
        return 0;
    }

    // RM WHITESPACE

    if (!is_function_body(input, &tmp, len))
    {
        return 0;
    }

    *index = tmp;
    return 1;
}


/*!
**  This function checks the newline_list grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 is the newline_list grammar is correct, 0 otherwise.
*/
int is_newline_list(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    size_t tmp2 = tmp;
    while (tmp < len && input[tmp] == '\n')
    {
        tmp += 1;
        struct token *to_add = init_token(T_NEWLINE, T_NONE, cut(input, &tmp2,
            tmp, len));
        add_token(lexer, to_add);
    }

    if (tmp == *index)
    {
        return 0;
    }

    *index = tmp;
    return 1;
}


/*!
**  This function checks the linebreak grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the linebreak grammar is correct, 0 otherwise.
*/
int is_linebreak(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    if (input[tmp] == '\0')
    {
        return 1;
    }

    if (!is_newline_list(input, &tmp, len))
    {
        return 0;
    }
    *index = tmp;
    return 1;
}


/*!
**  This function checks the command grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the command grammar is correct, 0 otherwise.
*/
int is_command(char *input, size_t *index, size_t len)
{
    if (is_simple_command(input, index, len))
    {
        return 1;
    }

    else if (is_compound_command(input, index, len))
    {
        if (is_redirect_list(input, index, len))
        {
            return 1;
        }

        return 1;
    }

    else if (is_function_definition(input, index, len))
    {
        return 1;
    }

    return 0;
}


/*!
**  This function chekcs the pipe_sequence grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the pipe_sequence grammar is correct, 0 otherwise.
*/
int is_pipe_sequence(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    if (!is_command(input, &tmp, len))
    {
        return 0;
    }

    // RM WHITESPACE

    while (tmp < len && input[tmp] == '|')
    {
        if (!is_linebreak(input, &tmp, len))
        {
            // GRAMMAR ERROR
            return 0;
        }
        if (!is_command(input, &tmp, len))
        {
            // GRAMMAR ERROR
            return 0;
        }
    }

    *index = tmp;
    return 1;
}

/*!
**  This function checks the pipeline grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the pipeline grammar is correct, 0 otherwise.
*/
int is_pipeline(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    if (tmp >= len || input[tmp] == '!')
    {
        tmp += 1;
    }

    if (!is_pipe_sequence(input, &tmp, len))
    {
        return 0;
    }

    *index = tmp;
    return 1;
}


/*!
**  This function checks the and_or grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the and_or grammar is correct, 0 otherwise.
*/
int is_and_or(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    int has_and_or = 0;
    if (!is_pipeline(input, &tmp, len))
    {
        return 0;
    }

    while (tmp >= len &&
            ((input[tmp] == '&' && input[tmp + 1] == '&') ||
            (input[tmp] == '|' && input[tmp + 1] == '|')))
    {
        has_and_or = 1;
        tmp += 2;
        // RM WHITE SPACE
        if (!is_linebreak(input, &tmp, len))
        {
            return 0;
        }
    }

    if (!is_pipeline(input, &tmp, len) && has_and_or)
    {
        return 0;
    }

    *index = tmp;
    return 1;
}


/*!
**  This function checks the term grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the term grammar is correct, 0 otherwise.
*/
int is_term(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    if (!is_and_or(input, &tmp, len))
    {
        return 0;
    }

    while (input[tmp] == ' ')
    {
        tmp++;
        is_and_or(input, &tmp, len);
    }

    /*while (is_separator(input, &tmp, len))
    {
        if (!is_and_or(input, &tmp, len))
        {
            return 0;
        }
    }*/

    *index = tmp;
    return 1;
}


/*!
**  This function checks the compound_list grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the compound_list grammar is correct, 0 otherwise.
*/
int is_compound_list(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    size_t tmp2 = tmp;
    if (is_term(input, &tmp, len))
    {
        struct token *to_add = init_token(T_COMMAND, T_NONE, cut(input, &tmp2,
        tmp, len));
        add_token(lexer, to_add);
        if (is_separator(input, &tmp, len))
        {
            *index = tmp;
            return 1;
        }
        *index = tmp;
        return 1;
    }
    else if (is_newline_list(input, &tmp, len))
    {
        if (is_term(input, &tmp, len))
        {
            struct token *to_add = init_token(T_COMMAND, T_NONE, cut(input,
            &tmp2, tmp, len));
            add_token(lexer, to_add);
            if (is_separator(input, &tmp, len))
            {
                *index = tmp;
                return 1;
            }
            *index = tmp;
            return 1;
        }
    }

    *index = tmp;
    return 0;
}

// DON'T FORGET TO ERASE WHITESPACE

/*!
**  This function checks the if_clause grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the if_clause grammar is correct, 0 otherwise.
*/
int is_if_clause(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    size_t tmp2 = tmp;
    if (tmp >= len - 1 || input[tmp] != 'i' || input[tmp + 1] != 'f')
    {
        return 0;
    }
    tmp += 2;

    struct token *to_add = init_token(T_IF, T_WORD, cut(input, &tmp2, tmp,
    len));
    add_token(lexer, to_add);

    if (tmp >= len || input[tmp] != ' ')
    {
        return 0;
    }
    tmp += 1;
    tmp2 = tmp;


    if (!is_compound_list(input, &tmp, len))
    {
        return 0;
    }


    remove_white_space(input, &tmp, len);

    tmp2 = tmp;
    if (tmp >= len - 3 || input[tmp] != 't' || input[tmp + 1] != 'h'
        || input[tmp + 2] != 'e' || input[tmp + 3] != 'n')
    {
        return 0;
    }
    tmp += 4;
    to_add = init_token(T_THEN, T_WORD, cut(input, &tmp2, tmp, len));
    add_token(lexer, to_add);

    if (!is_compound_list(input, &tmp, len))
    {
        return 0;
    }

    if (!is_else_part(input, &tmp, len))
    {
        if (tmp >= len - 1 || input[tmp] != 'f' || input[tmp + 1] != 'i')
        {
            return 0;
        }
        tmp += 2;

        to_add = init_token(T_FI, T_WORD, cut(input, &tmp2, tmp, len));
        add_token(lexer, to_add);

        *index = tmp;
        return 1;
    }

    if (tmp >= len - 1 || input[tmp] != 'f' || input[tmp + 1] != 'i')
    {
        return 0;
    }
    tmp2 = tmp;
    tmp += 2;
    to_add = init_token(T_FI, T_WORD, cut(input, &tmp2, tmp, len));
    add_token(lexer, to_add);
    *index = tmp;
    return 1;
}


/*!
 **  This function checks the elif/then grammar.
 **  \param input : the string to be checked.
 **  \param index : the current index in the string input.
 **  \param len : the actual length of the input string.
 **  \return 1 is the elif/then grammar is correct, 0 otherwise.
 */
int is_elif_then(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    /*if (input[tmp] != 'i' && input[tmp + 1] != 'f')
    {
        return 0;
    }
    tmp += 2;*/

    if (tmp >= len || input[tmp] != ' ')
    {
        return 0;
    }
    tmp += 1;
    size_t tmp2 = tmp;

    if (is_compound_list(input, &tmp, len) == 0)
    {
        return 0;
    }

    if (tmp >= len - 3 || input[tmp] != 't' || input[tmp + 1] != 'h'
            || input[tmp + 2] != 'e' || input[tmp + 3] != 'n')
    {
        return 0;
    }
    tmp += 4;
    struct token *to_add = init_token(T_THEN, T_WORD, cut(input,
    &tmp2, tmp, len));
    add_token(lexer, to_add);
    tmp2 = tmp;

    if (!is_compound_list(input, &tmp, len))
    {
        return 0;
    }

    if (is_else_part(input, &tmp, len) == 0)
    {
        return 0;
    }

    *index = tmp;
    return 1;
}


/*!
 **  This function checks else grammar.
 **  \param input : the string to be checked.
 **  \param index : the current index in the string input.
 **  \param len : the actual length of the input string.
 **  \return 1 if the else grammar is correct, 0 otherwise.
 */
int is_else_part(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    size_t tmp2 = tmp;
    if (tmp >= len - 1 || input[tmp] != 'e' || input[tmp + 1] != 'l')
    {
        return 0;
    }
    tmp += 2;

    if (tmp >= len || (input[tmp] != 'i' && input[tmp] != 's'))
    {
        return 0;
    }
    tmp += 1;

    if (tmp >= len || (input[tmp] != 'f' && input[tmp] != 'e'))
    {
        return 0;
    }
    tmp += 1;

    if (is_elif_then(input, &tmp, len))
    {
        *index = tmp;
        return 1;
    }

    struct token *to_add = init_token(T_ELSE, T_WORD, cut(input, &tmp2,
            tmp, len));
    add_token(lexer, to_add);
    tmp2 = tmp;

    /*if (input[tmp] != 's' || input[tmp + 1] != 'e')
    {
        return 0;
    }*/

    if (!is_compound_list(input, &tmp, len))
    {
        return 0;
    }

    *index = tmp;
    return 1;
}


/*!
**  This function checks is the input meets the requirements for a name.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the input is a valid word, 0 otherwise.
*/
int is_WORD(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    if (tmp >= len || ((input[tmp] >= 48 && input[tmp] <= 57)
        || isspace(input[tmp])))
    {
        return 0;
    }
    tmp += 1;

    while (tmp < len && !(isspace(input[tmp])))
    {
        if (input[tmp] >= 1 && input[tmp] <= 6)
            return 0;
        if (input[tmp] >= 14 && input[tmp] <= 31)
            return 0;
        if (input[tmp] == 127)
            return 0;

        tmp++;
    }
    *index = tmp;
    return 1;
}


/*!
**  This function checks the separator_op grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the separator_op grammar is correct, 0 otherwise.
*/
int is_separator_op(char *input, size_t *index, size_t len)
{
    if (*index >= len || (input[*index] != '&' && input[*index] != ';'))
    {
        return 0;
    }
    *index += 1;
    return 1;
}


/*!
**  This function checks the separator grammar.
**  \param input : the string to be checked.
**  \param index : the current index in the input string.
**  \param len : the actual length of the input string.
**  \return 1 if the separator grammar is correct, 0 otherwise.
*/
int is_separator(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    if (is_separator_op(input, &tmp, len) == 0
        && is_linebreak(input, &tmp, len) == 0)
    {
        if (is_newline_list(input, &tmp, len) == 0 && input[tmp] != ' ')
        {
            return 0;
        }

        *index = tmp;
        return 1;
    }

    *index = tmp;
    return 1;
}


/*!
**  This function computes the length of the string starting at a given index.
**  \param input : the string whose length we want to know.
**  \param index : the index where we want to start.
**  \return The length of the input starting at index.
*/
size_t my_strlen(char *input, size_t *index)
{
    size_t tmp = *index;
    size_t result = 0;
    while (input[tmp] != '\0')
    {
        tmp++;
        result++;
    }
    return result;
}


/*int main(void)
{
    lexer = init_token_list();
    char *tmp = reserved_words[1];
    tmp = tmp;
    size_t index = 0;
    //char *str1 = "if lol\nthen\ndosomething\nfi";
    char *str2 = "if echo lol\nthen\necho foo\nelse\necho bar\nfi";
    char *str3 = "if echo lol\nthen\necho foo\nelif echo ptdr\nthen\necho suce\nelse\necho bar\nfi";
    char *str4 = "echo lol";
    char *str5 = "echo lol || echo xavier le fils de pute\n";
    //printf("is_if_case %d\n", is_if_clause(str1, &index, strlen(str1)));
    //token_printer(lexer);
    //index = 0;
    printf("is_if__else_case: %d\n", is_if_clause(str2, &index, strlen(str2)));
    token_printer(lexer);
    index = 0;
    printf("is_if_elif_else_case: %d\n", is_if_clause(str3, &index,
    strlen(str3)));
    index = 0;
    printf("is_simple_command: %d\n", is_simple_command(str4, &index,
    strlen(str4)));
    index = 0;
    printf("is_compound_command: %d\n", is_compound_list(str5, &index,
    strlen(str5)));
    printf("index: %zu\n", index);
}*/
