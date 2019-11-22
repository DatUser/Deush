/*!
**  \brief
**  This file contains all the functions that are related to the parsing
**  \file parser.c
**  \author Xavier Gomez
**/
#include <stdlib.h>
#include "header/astconvert.h"
#include <stdio.h>
#include <string.h>
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
    //token_printer(lexer);
    //return 0;
    if (lexer->head)
    {
        switch (lexer->head->primary_type)
        {
        case T_COMMAND://replace by T_COMMAND whenever you merge
            return parse_command(ast);
        case T_IF:
            return parse_if(ast, 1);
        case T_WHILE:
            return parse_while(ast);
        case T_FOR:
            return parse_for(ast);
        case T_CASE:
            return parse_case(ast);
        default:
            break;
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

int parse_single_command(struct ast **ast)
{
    if (lexer->head)
    {
        if (lexer->head && lexer->head->primary_type != T_DSEMI)
        {
            int len = strlen(lexer->head->value);
            lexer->head->value[len - 1] = '\0';
            /*struct token *tmp = pop_lexer();
            free(tmp->value);
            free(tmp);*/
            //printf("Lexer is not NULL\n");
            struct ast *child_cmd = create_node_lexer();
            struct ast *child_separator = create_node_lexer();
            add_child(*ast, child_separator);
            add_child(child_separator, child_cmd);
            parse(&child_cmd);
        }
        return 0;
    }
    return 1;

}

int parse_command(struct ast **ast)
{
    if (lexer->head)
    {
        //eat separator
        while (lexer->head && lexer->head->primary_type == T_COMMAND)
        {
            /*struct token *tmp = pop_lexer();
            free(tmp->value);
            free(tmp);*/
            //printf("Lexer is not NULL\n");
            struct ast *child_cmd = create_node_lexer();
            while (lexer->head->secondary_type == T_PIPE
                    || lexer->head->secondary_type == T_ORIF
                    || lexer->head->secondary_type == T_ANDIF)
                parse_pipe(&child_cmd);

            struct ast *child_separator = create_node_lexer();
            add_child(*ast, child_separator);
            add_child(child_separator, child_cmd);
        }
        parse(ast);
        return 0;
    }
    return 1;
}

/*!
**  Creates a node obtained by parsing a list of word (after in of for loop)
**  \param ast : Address of the tree
**  \return On success 0, 1 otherwise
**/
int parse_wordlist(struct ast **ast)
{
    if (lexer->head)
    {
        while (lexer->head && lexer->head->primary_type != T_SEPARATOR)
        {
            struct ast *child_cmd = create_node_lexer();
            add_child(*ast, child_cmd);
        }
        return 0;
    }
    return 1;
}

int parse_in_for(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        parse_wordlist(&child);
        //eat separator beacause it as ';' and it is useless
        struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);
    }
    return 0;
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
        //eat separator
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
        //eat separator
        /*struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);*/

        //parse_next_token(&child);//separator
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
        struct ast *child = create_node_lexer();//if
        add_child(*ast, child);
        int out = 0;
        out = (out) ? out : parse_command/*next_token*/(&child);//condition
        //out = (out) ? out : parse_next_token(&child);//separator
        out = (out) ? out : parse_then(&child);//then

        if (lexer->head->primary_type == T_ELIF)
            out = (out) ?  out : parse_if(&child, 0);//elif
        if (lexer->head->primary_type == T_ELSE && is_if)
            out = (out) ? out : parse_then(&child);//else
        out = (out) ? out : parse_next_token(&child);//fi
        //eat separator
        struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);
        return 0;
    }
    return is_if;
}
int parse_in_case(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast,child);
         int out = 0;
        //eat separator
        struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);
        while (lexer->head->primary_type != T_ESAC)
        {
            out = (out) ? out : parse_single_command(&child);
        }
        return 0;
    }
    return 0;
}
int parse_case(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        int out = 0;
        out = (out) ? out : parse_next_token(&child);//variable before in
        out = (out) ? out : parse_in_case(&child);//the in

        out = (out) ? out : parse_next_token(&child);//the essac at the end
        return 0;
    }
    return 0;
}

int parse_do(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        int out = 0;
        //eat separator
        /*struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);*/
        out = (out) ? out : parse(&child);//every command in the while
        //out = (out) ? out : parse_command(&child);//the done at the end
    }
    return 0;
}

int parse_while(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast,child);
        int out = 0;
        out = (out) ? out : parse_command(&child);//command inside while
        out = (out) ? out : parse_do(&child);//everythings insides
        out = (out) ? out : parse_next_token(&child);//done
        return 0;
    }
    return 0;
}

int parse_for(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        int out = 0;
        out = (out) ? out : parse_next_token(&child); //variable before the in
        out = (out) ? out : parse_in_for(&child);//parses the in
        out = (out) ? out : parse_do(&child);//parses the do and what is inside
        out = (out) ? out : parse_next_token(&child); //parses the done
    }
    return 0;
}

/*!
**  Parses the T_PIPE token, (as the **ast is the address of the left command
**  of the pipe this functions creates a node for pipe that will replace the
**  left command and the left command will be the 1st child of the pipe)
**/
int parse_pipe(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child_pipe = create_node_lexer();
        struct ast *second_command = create_node_lexer();
        struct ast *first_command = *ast;

        *ast = child_pipe;
        add_child(*ast, first_command);
        add_child(*ast, second_command);

        /*int out = 0;
        out = (out) ? out : parse_command(&child);*/
    }
    return 0;
}
