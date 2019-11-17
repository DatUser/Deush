/*!
**  \brief
**  This file contains all the functions that are related to the parsing
**  \file parser.c
**  \author Xavier Gomez
**/
#include <stdlib.h>
#include "header/astconvert.h"

/*!
**  Inits the lexer
**/
void init_lexer(void)
{
    static int first = 1;
    if (first)
    {
        first = 0;
        lexer = NULL;
    }
}

/*!
**  Creates the tree obtained by parsing the lexer's token
**  \return On success 0, 1 otherwise
**/
int parse(struct ast **ast)
{
    //struct ast *ast = NULL;

    if (lexer->head)
    {
        switch (lexer->head->primary_type)
        {
        case T_COMMAND://replace by T_COMMAND whenever you merge
            return parse_command(ast);
        case T_IF:
            return parse_if(ast, 1);
        }
        return 1;
    }
    return 0;
}

/*!
**  Creates a node with the type of the current lexer token  and pops it
**  \return The node created
**/
struct ast *create_node_lexer(void)
{
    struct ast *node = create_node(lexer->head->value,
        lexer->head->primary_type);
    struct token *out = pop_lexer();
    free(out);
    return node;
}

/*!
**  Creates a node obtained by parsing a command and adds it in the tree
**  \param ast : Address of the tree
**  \return On success 0, 1 otherwise
**/
int parse_command(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        return 0;
    }
    return 1;
}

/*!
**  Creates a node obtained by parsing a default token and adds it in the tree
**  as child
**  \param ast : Address of the tree
**  \return On success 0, 1 otherwise
**/
int parse_next_token(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        return 0;
    }
    return 1;
}

/*!
**  Creates a node obtained by parsing an then rule and adds it in the tree
**  as child
**  \param ast : Address of the tree
**  \return On success 0, 1 otherwise
**/
int parse_then(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        parse(&child);
        return 0;
    }
    return 1;
}

/*!
**  Creates a node obtained by parsing an if rule and adds it in the tree
**  as child
**  \param ast : Address of the tree
**  \param is_if : specify if the function is called from if token or elif.
**  (1 -> if_token , 0 otherwise)
**  \return On success 0, 1 otherwise
**/
int parse_if(struct ast **ast, int is_if)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        int out = 0;
        out = (out) ? out : parse_next_token(&child);//condition
        out = (out) ? out : parse_next_token(&child);//separator
        out = (out) ? out : parse_then(&child);//then

        if (lexer->head->primary_type == T_ELIF)
            out = (out) ?  out : parse_if(&child, 0);//elif
        if (lexer->head->primary_type == T_ELSE)
            out = (out) ? out : parse_then(&child);//else
        out = (out) ? out : parse_next_token(&child);//fi
        return 0;
    }
    return is_if;
}
