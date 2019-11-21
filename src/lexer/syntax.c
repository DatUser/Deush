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
