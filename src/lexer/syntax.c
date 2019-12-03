/*!
**  \file syntax.c
**  \brief This file contains all the function related to the syntax of the
**  token stored in the token list.
**  \author 42sh Group
*/

#include "header/token.h"
#include "../include/include.h"
#include "header/lexer.h"

/*!
**  This function checks if the tokens stored in the token list are a pipeline.
**  \return 1 if this is a pipeline, 0 otherwise.
*/
int is_pipeline(void)
{
    struct token *tmp = lexer->head;

    if (tmp != NULL)
    {
        if (tmp->primary_type == T_BANG)
        {
            tmp = tmp->next;
        }
    }

    if (tmp->primary_type != T_COMMAND)
    {
        return 0;
    }

    tmp = tmp->next;

    if (tmp->primary_type == T_NEWLINE)
    {
        return 1;
    }

    enum token_type prev = T_NONE;
    while (tmp)
    {
        if (tmp->primary_type == T_NEWLINE)
        {
            return 1;
        }

        if (tmp->primary_type != T_PIPE && prev == T_COMMAND)
        {
            return 0;
        }

        prev = tmp->primary_type;
        tmp = tmp->next;

        if (tmp && tmp->primary_type == T_NEWLINE)
        {
            return 1;
        }

        if (tmp && tmp->primary_type != T_COMMAND && prev == T_PIPE)
        {
            return 0;
        }

        tmp = tmp->next;
    }

    return 1;
}

/*!
**  This function checks the syntax of the 'for' condition.
**  \param actual : The actual token the function is reviewing.
**  \param error : The variable setting an error.
**  \return NULL, and sets error to 1 if the syntax is wrong.
*/
struct token *for_check(struct token *actual, int *error)
{
    if (actual->primary_type != T_FOR)
    {
        return actual;
    }
    actual = actual->next;

    if (actual->primary_type != T_WORD)
    {
        if (actual->secondary_type == T_WORD)
        {
            actual->primary_type = T_WORD;
        }
        else
        {
            *error = 1;
            return actual;
        }
    }
    actual = actual->next;

    if (actual->primary_type == T_SEPARATOR || actual->primary_type == T_IN)
    {
        if (actual->secondary_type == T_SEMI)
        {
            actual = actual->next;
        }
        else if (actual->secondary_type == T_NEWLINE
                    || actual->primary_type == T_IN)
        {
            while (actual && actual->secondary_type == T_NEWLINE)
            {
                actual = actual->next;
            }
            if (actual->primary_type != T_IN)
            {
                *error = 1;
                return actual;
            }
            actual = actual->next;
            while (actual)
            {
                if (actual->primary_type == T_WORD ||
                    actual->secondary_type == T_WORD)
                {
                    actual->primary_type = T_WORD;
                    actual = actual->next;
                }
                else
                {
                    break;
                }
            }
            if (actual->primary_type != T_SEPARATOR
                && actual->secondary_type != T_SEMI
                && actual->secondary_type != T_NEWLINE)
            {
                *error = 1;
                return actual;
            }
            actual = actual->next;
        }
    }
    else
    {
        *error = 1;
        return actual;
    }

    while (actual && actual->primary_type == T_SEPARATOR
            && actual->secondary_type == T_NEWLINE)
    {
        actual = actual->next;
    }

    actual = do_check(actual, error);
    return actual;
}


/*!
**  This function checks the syntax of the 'do' condition.
**  \param actual : The actual token the function is reviewing.
**  \param error : The variable setting an error.
**  \return NULL, and sets error to 1 if the syntax is wrong.
*/
struct token *do_check(struct token *actual, int *error)
{
    if (actual == NULL)
    {
        return NULL;
    }
    if (actual->primary_type != T_DO)
    {
        return actual;
    }
    actual = actual->next;
    while (actual)
    {
        if (actual->primary_type == T_DONE)
            return actual->next;
        if (actual->primary_type == T_WORD
            && actual->next->primary_type == T_SEPARATOR)
        {
            actual->primary_type = T_COMMAND;
        }
        else if (actual->primary_type == T_BUILTIN
                || actual->primary_type == T_COMMAND)
        {
            while (actual && actual->primary_type != T_SEPARATOR)
            {
                actual = actual->next;
            }
            if (!actual)
                break;
            else
                actual = actual->next;
        }
        else
        {
            break;
        }
    }
    *error = 1;
    return NULL;
}

/*!
**  This function checks the syntax of the 'elif' condition.
**  \param actual : The actual token the function is reviewing.
**  \param error : The variable setting an error.
**  \return NULL, and sets error to 1 if the syntax is wrong.
*/
struct token *elif_check(struct token *actual, int *error)
{
    if (!actual)
    {
        return NULL;
    }
    if (actual->primary_type != T_ELIF)
    {
        return actual;
    }
    actual = command_check(actual->next, error);
    if (actual == NULL)
    {
        return NULL;
    }
    if (actual->primary_type != T_THEN)
    {
        *error = 1;
        return NULL;
    }
    actual = command_check(actual->next, error);
    if (actual == NULL)
    {
        return NULL;
    }
    return actual;
}

/*!
**  This function checks the syntax of the 'else' condition.
**  \param actual : The actual token the function is reviewing.
**  \param error : The variable setting an error.
**  \return NULL, and sets error to 1 if the syntax is wrong.
*/
struct token *else_check(struct token *actual, int *error)
{
    if (!actual)
    {
        return NULL;
    }
    if (actual->primary_type != T_ELSE)
    {
        return actual;
    }
    actual = actual->next;
    while (actual && actual->primary_type == T_SEPARATOR)
    {
        if (actual->secondary_type == T_SEMI)
        {
            actual = actual->next;
            break;
        }
        actual = actual->next;
    }
    actual = command_check(actual, error);
    if (actual == NULL)
    {
        return NULL;
    }
    return actual;
}

/*!
**  This function checks the syntax of the 'if' condition.
**  \param actual : The actual token the function is reviewing.
**  \param error : The variable setting an error.
**  \return NULL, and sets error to 1 if the syntax is wrong.
*/
struct token *if_check(struct token *actual, int *error)
{
    if (!actual)
    {
        return NULL;
    }
    if (actual->primary_type != T_IF)
    {
        return actual;
    }
    actual = actual->next;

    actual = command_check(actual, error);
    if (actual == NULL)
    {
        return NULL;
    }

    if (actual->primary_type != T_THEN)
    {
        *error = 1;
        return NULL;
    }
    actual = actual->next;

    actual = command_check(actual, error);
    if (actual == NULL)
    {
        return NULL;
    }
    struct token *tmp = actual;
    do
    {
        actual = elif_check(actual, error);
        if (actual != tmp)
        {
            tmp = actual;
        }
        else
        {
            break;
        }
    } while (actual == tmp);
    //actual = elif_check;
    actual = else_check(actual, error);
    if (actual->primary_type != T_FI)
    {
        *error = 1;
        return NULL;
    }
    return actual->next;
}

/*!
**  This function checks the syntax of the command.
**  \param actual : The actual token the function is reviewing.
**  \param error : The variable setting an error.
**  \return NULL, and sets error to 1 if the syntax is wrong.
*/
struct token *command_check(struct token *actual, int *error)
{
    if (!actual)
    {
        return NULL;
    }
    if (actual->primary_type != T_COMMAND && actual->primary_type != T_BUILTIN)
    {
        return actual;
    }
    actual = actual->next;
    while (actual && actual->primary_type != T_SEPARATOR)
    {
        if (actual->primary_type != T_WORD && actual->secondary_type != T_WORD)
        {
            *error = 1;
        }
        actual = actual->next;
    }
    while (actual && (actual->primary_type == T_SEPARATOR
            || actual->secondary_type == T_NEWLINE))
    {
        actual = actual->next;
    }
    return actual;
}

struct token *case_item_check(struct token *actual, int *error);

/*!
**  This function checks the syntax of the 'case_clause' condition.
**  \param actual : The actual token the function is reviewing.
**  \param error : The variable setting an error.
**  \return NULL, and sets error to 1 if the syntax is wrong.
*/
struct token *case_clause_check(struct token *actual, int *error)
{
    actual = case_item_check(actual, error);
    while (actual->secondary_type == T_DSEMI)
    {
        actual = actual->next;
        while (actual->secondary_type == T_NEWLINE)
        {
            actual = actual->next;
        }
        actual = case_item_check(actual, error);
    }
    if (actual->secondary_type == T_DSEMI)
    {
        actual = actual->next;
    }
    while (actual->secondary_type == T_NEWLINE)
    {
        actual = actual->next;
    }
    return actual;
}

/*!
**  This function checks the syntax of the 'case_item' condition.
**  \param actual : The actual token the function is reviewing.
**  \param error : The variable setting an error.
**  \return NULL, and sets error to 1 if the syntax is wrong.
*/
struct token *case_item_check(struct token *actual, int *error)
{
    if (!actual)
    {
        return NULL;
    }
    struct token *tmp = actual;
    if (actual->primary_type == T_LPAR)
    {
        actual = actual->next;
    }

    if (actual->primary_type != T_WORD)
    {
        if (tmp->secondary_type == T_LPAR)
        {
            *error = 1;
            return actual;
        }
        return tmp;
    }
    actual = actual->next;

    while (actual->secondary_type == T_PIPE)
    {
        actual = actual->next;
        if (actual->primary_type != T_WORD && actual->secondary_type != T_WORD)
        {
            *error = 1;
            return NULL;
        }
        actual->primary_type = T_WORD;
        actual = actual->next;
    }

    if (actual->secondary_type != T_RPAR)
    {
        *error = 1;
        return actual;
    }
    actual = actual->next;

    while (actual->secondary_type == T_NEWLINE)
    {
        actual = actual->next;
    }

    actual = command_check(actual, error);
    return actual;
}

/*!
**  This function checks the syntax of the 'case' condition.
**  \param actual : The actual token the function is reviewing.
**  \param error : The variable setting an error.
**  \return NULL, and sets error to 1 if the syntax is wrong.
*/
struct token *case_check(struct token *actual, int *error)
{
    if (actual == NULL)
    {
        return NULL;
    }
    if (actual->primary_type != T_CASE)
    {
        return actual;
    }
    actual = actual->next;

    if (actual->primary_type != T_WORD && actual->secondary_type != T_WORD)
    {
        *error = 1;
        return actual;
    }
    actual->primary_type = T_WORD;
    actual = actual->next;
    while (actual->secondary_type == T_NEWLINE)
    {
        actual = actual->next;
    }
    if (actual->primary_type != T_IN)
    {
        *error = 1;
        return actual;
    }
    while (actual->secondary_type == T_NEWLINE)
    {
        actual = actual->next;
    }
    actual = case_clause_check(actual, error);
    if (actual->primary_type != T_ESAC)
    {
        *error = 1;
    }
    return actual;
}

/*!
**  This function checks the syntax of the 'for', 'while', 'until' conditions.
**  \param actual : The actual token the function is reviewing.
**  \param error : The variable setting an error.
**  \return NULL, and sets error to 1 if the syntax is wrong.
*/
struct token *for_while_until(struct token *actual, int *error)
{
    if (actual == NULL)
    {
        return NULL;
    }
    if (actual->primary_type != T_FOR && actual->primary_type != T_WHILE
        && actual->primary_type != T_UNTIL)
    {
        return actual;
    }
    while (actual)
    {
        if (actual->primary_type == T_DO)
        {
            return do_check(actual, error);
        }
        actual = actual->next;
    }
    *error = 1;
    return NULL;
}


/*!
**  This function functions checks if there is a token matching the name
**  parameter.
**  \param name : the name of the function we want to find.
**  \return 1 if there is a token matching the name parameter, 0 otherwise.
*/
int is_function_name(char *name)
{
    struct token *tmp = lexer->head;
    while (tmp)
    {
        if (tmp->primary_type == T_FUNCTION && tmp->next)
        {
            if (!strcmp(tmp->next->value, name))
            {
                return 1;
            }
        }
        tmp = tmp->next;
    }
    return 0;
}


/*!
**  This function checks the syntax of the 'while' condition.
**  \param actual : the actual token the function is reviewing.
**  \param error : the variable setting an error.
**  \return the next token to be reviewed if it is a 'while' condtion, NULL
**  otherwise.
*/
struct token *while_check(struct token *actual, int *error)
{
    if (!actual)
    {
        return NULL;
    }
    if (actual->primary_type != T_WHILE)
    {
        return actual;
    }
    actual = actual->next;

    actual = command_check(actual, error);
    if (actual == NULL)
    {
        *error = 1;
        return NULL;
    }

    if (actual->primary_type != T_DO)
    {
        *error = 1;
        return NULL;
    }
    actual = actual->next;

    actual = command_check(actual, error);
    if (actual == NULL)
    {
        return NULL;
    }

    if (actual->primary_type != T_DONE)
    {
        *error = 1;
        return NULL;
    }
    return actual->next;
}


/*!
**  This function checks the syntax of the 'until' condition.
**  \param actual : the actual token the function is reviewing.
**  \param error : the variable setting an error.
**  \return the next token to be reviewed if it is an 'until' condtion, NULL
**  otherwise.
*/
struct token *until_check(struct token *actual, int *error)
{
    if (!actual)
    {
        return NULL;
    }
    if (actual->primary_type != T_UNTIL)
    {
        return actual;
    }
    actual = actual->next;

    actual = command_check(actual, error);
    if (actual == NULL)
    {
        *error = 1;
        return NULL;
    }

    if (actual->primary_type != T_DO)
    {
        *error = 1;
        return NULL;
    }
    actual = actual->next;

    actual = command_check(actual, error);
    if (actual == NULL)
    {
        return NULL;
    }

    if (actual->primary_type != T_DONE)
    {
        *error = 1;
        return NULL;
    }
    return actual->next;
}


/*!
**  This function checks the grammar of the tokens stored in the token list.
**  \return 1 if there is no errors in the grammar, 1 otherwise.
*/
int is_good_grammar(void)
{
    if (lexer->head == NULL)
        return 1;
    struct token *actual = lexer->head;
    int error = 0;
    while (actual)
    {
        actual = for_check(actual, &error);
        actual = if_check(actual, &error);
        actual = command_check(actual, &error);
        actual = case_check(actual, &error);
        //actual = for_while_until(actual, &error);
        actual = while_check(actual, &error);
        actual = until_check(actual, &error);
        if (actual && actual->primary_type == T_FUNCTION)
        {
            actual = actual->next->next;
        }
        if (actual && actual->primary_type == T_WORD)
        {
            if (is_function_name(actual->value))
            {
                actual->primary_type = T_FUNCTION_NAME;
            }
            else if (actual->secondary_type != T_EXPAND)
            {
                actual->primary_type = T_COMMAND;
            }
        }
        if (actual)
        {
            actual = actual->next;
        }
    }
    return error;
}
