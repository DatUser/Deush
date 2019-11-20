#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "header/astconvert.h"
#include "header/stringutils.h"
#include "../prompt/header/prompt.h"

/*!
**  Evaluates a node that contains a command and runs it
**  \param ast : Node that contains the command
**  \return The return value of the command executed
**/
int eval_command(struct ast *ast)
{
    size_t len = 0;
    char **command = cut_line(ast->child->node->data, &len);
    int out = execution(command, command[0]);
    free(command);
    return out;
}

/*!
**  Evaluates all the children of a node
**/
int eval_children(struct ast *ast)
{
    struct node_list *tmp = ast->child;

    while (tmp)
    {
        eval_ast(tmp->node);
        tmp = tmp->next;
    }
    return 0;
}

/*!
**  Finds the a node of type type starting at index *i and saves where it
**  stopped seeking at address
**  \param children : List of nodes
**  \param type : Type wanted
**  \param i : Address of the index
**  \return The first with wanted type found if it exits, NULL otherwise
**/
struct ast *find_node(struct node_list *children, enum token_type type, int *i)
{
    struct node_list *tmp = children;
    int j = 0;
    while (j != *i)
        j++;
    while (tmp && tmp->node->type != type)
    {
        j++;
        tmp = tmp->next;
    }
    if (tmp)
    {
        *i = j;
        return tmp->node;
    }

    return NULL;
}

/*!
**  Evaluates a node that contains of type if
**  \param ast : Node that contains the command
**  \return The return value of the command executed, 0 if no command is
**  command
**/
int eval_if(struct ast *ast)
{
    int i = 0;
    struct ast *condition_node = find_node(ast->child, T_SEPARATOR, &i);
    if (!eval_command(condition_node))
    {
        struct ast *then_node = find_node(ast->child, T_THEN, &i);
        return eval_children(then_node);//eval_ast(then_node->child->node);
    }
    struct ast *elif_node = NULL;
    int elif_pa = 0;
    while ((elif_node = find_node(ast->child, T_ELIF, &i)) != NULL && !elif_pa)
    {
        if (!eval_command(elif_node->child->node))//test condition
        {
            elif_pa = 1;
            return eval_children(elif_node);//eval_ast(elif_node->child->node);
        }
    }
    struct ast *else_node = find_node(ast->child, T_ELSE, &i);
    if (else_node)
        return eval_children(else_node);//eval_ast(else_node->child->node);
    return 0;
}

int eval_while(struct ast *ast)
{
    int i = 0;
    struct ast *condition_node = find_node(ast->child, T_SEPARATOR, &i);
    while (!eval_command(condition_node))
        eval_children(condition_node);
    return 0;
}
/*!
**  Evaluates a node that contains an unknown type
**  \param ast : Node
**  \return The return value of the node evalutaion
**  command
**/
int eval_ast(struct ast *ast)
{
    if (ast)
    {
        switch (ast->type)
        {
        case T_SEPARATOR:
            return eval_command(ast);
        case T_IF:
            return eval_if(ast);
        case T_WHILE:
            return eval_while(ast);
        default:
            return 0;
        }
    }
    return 0;
}
