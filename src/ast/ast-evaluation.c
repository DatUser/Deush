#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "../include/global.h"
#include "header/astconvert.h"
#include "header/stringutils.h"
#include "../prompt/header/prompt.h"
#include "header/builtin_exec.h"
#include "../substitution/header/assignement_variables.h"



char *shopt_opt[8] = {"ast_print", "dotglob", "expand_aliases","extglob",
                           "nocaseglob", "nullglob", "sourcepath", "xpg_echo"};

int shopt_opt_nbr[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int eval_operator_redirection(struct ast *ast, int *evaluated)
{
    char *separator = ast->child->node->data;
    if (separator[0] == '&' && separator[1] == '&')
        return eval_and(ast);
    if (separator[0] == '|' && separator[1] == '|')
        return eval_or(ast);
    if (separator[0] == '|')
        return eval_pipe(ast);
    if (ast->child->node->type == T_LESS)
        return eval_redirect_left(ast, extract_nb(separator));
    if (ast->child->node->type == T_GREATER
        || ast->child->node->type == T_CLOBBER
        || ast->child->node->type == T_RGREAT)
        return eval_redirect_right(ast, extract_nb(separator));
    if (ast->child->node->type == T_LESSGREAT)
        return eval_redirect_both(ast, extract_nb(separator));
    if (ast->child->node->type == T_RLESS)
        return eval_redirect_double_left(ast, extract_nb(separator));
    if (ast->child->node->type == T_GREATAND)
        return eval_redirect_right_and(ast);
    if (ast->child->node->type == T_LESSAND)
        return eval_redirect_left_and(ast);

    *evaluated = 0;
    return 0;
}
/*!
**  Evaluates a node that contains a command and runs it
**  \param ast : Node that contains the command
**  \return The return value of the command executed
**/
int eval_command(struct ast *ast)
{
    int evaluated = 1;
    int return_value = eval_operator_redirection(ast, &evaluated);

    if (evaluated)
        return return_value;
    /*char *separator = ast->child->node->data;
    if (separator[0] == '&' && separator[1] == '&')
        return eval_and(ast);
    if (separator[0] == '|' && separator[1] == '|')
        return eval_or(ast);
    if (separator[0] == '|')
        return eval_pipe(ast);
    if (ast->child->node->type == T_LESS)
        return eval_redirect_left(ast, extract_nb(separator));
    if (ast->child->node->type == T_GREATER
        || ast->child->node->type == T_CLOBBER
        || ast->child->node->type == T_RGREAT)
        return eval_redirect_right(ast, extract_nb(separator));
    if (ast->child->node->type == T_LESSGREAT)
        return eval_redirect_both(ast, extract_nb(separator));
    if (ast->child->node->type == T_RLESS)
        return eval_redirect_double_left(ast, extract_nb(separator));
    if (ast->child->node->type == T_GREATAND)
        return eval_redirect_right_and(ast);*/
    size_t len = 0;
    void *copy = strdup(ast->child->node->data);
    char **command = cut_line(copy, &len);
    int out = execution(command, command[0]);
    free(copy);
    free(command);
    return out;
}

/*!
**  Evaluates all the conditions of a if/while node
**/
int eval_conditions(struct ast *ast)
{
    int out = 0;
    struct node_list *conditions = ast->child;

    while (conditions->node->type == T_SEPARATOR)
    {
        out = (out) ? out : eval_command(conditions->node);
        conditions = conditions->next;
    }

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

/*!int main(void)
{
    variable_update("salut","2");
    variable_update("popo", "${salut}");
    variable_update("even", "caca");
    variable_update("final","${popo}");
    check_substitution();
    print_variables();
    
}
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
**  Evaluates a node that is type if
**  \param ast : Node that contains the command
**  \return The return value of the command executed, 0 if no command is
**  command
**/
int eval_case(struct ast *ast)
{
    int i = 0;
    struct node_list *variable_eval = ast->child;
    struct node_list *cases = find_node(ast->child, T_IN, &i)->child;
    while (cases && (strcmp(variable_eval->node->data,
            cases->node->child->node->data) != 0)
            && (strcmp(cases->node->data, "*") != 0))
    {
        cases = cases->next;
        cases = cases->next;
    }
    eval_children(cases->node->child->node);
    return 0;

}
int eval_if(struct ast *ast)
{
    int i = 0;
    //struct ast *condition_node = find_node(ast->child, T_SEPARATOR, &i);
    if (!eval_conditions(ast)/*eval_command(condition_node)*/)
    {
        struct ast *then_node = find_node(ast->child, T_THEN, &i);
        return eval_children(then_node);//eval_ast(then_node->child->node);
    }
    struct ast *elif_node = NULL;
    int elif_pa = 0;
    while ((elif_node = find_node(ast->child, T_ELIF, &i)) != NULL && !elif_pa)
    {
        if (!eval_conditions(elif_node)/*eval_command(elif_node->child->node)*/)//test condition
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

/*!
**  Evaluates a node that i of type while
**  \param ast : Node of type while
**  \return The return value is 0 by default
**/
int eval_while(struct ast *ast)
{
    int i = 0;
    //struct ast *condition_node = find_node(ast->child, T_SEPARATOR, &i);
    struct ast *do_node = find_node(ast->child, T_DO, &i);
    while (!eval_conditions(ast)/*eval_command(condition_node)*/)
        eval_children(do_node);
    return 0;
}

/*!
**  Evaluates a node that i of type while
**  \param ast : Node of type while
**  \return The return value is 0 by default
**/
int eval_until(struct ast *ast)
{
    int i = 0;
    //struct ast *condition_node = find_node(ast->child, T_SEPARATOR, &i);
    struct ast *do_node = find_node(ast->child, T_DO, &i);
    while (eval_conditions(ast)/*eval_command(condition_node)*/)
        eval_children(do_node);
    return 0;
}

/*!
**  Evaluates a node that is of type for
**  \param ast : Node of type for
**  \return The return value is 0 by default
**/
int eval_for(struct ast *ast)
{
    int i = 0;
    struct ast *in_node = find_node(ast->child, T_IN, &i);
    struct ast *do_node = find_node(ast->child, T_DO, &i);

    struct node_list *tmp = in_node->child;

    while (tmp)
    {
        eval_children(do_node);
        tmp = tmp->next;
    }

    return 0;
}

/*!
**  Evaluates pipe
**/
int eval_pipe(struct ast *ast)
{
    int fd[2];
    pipe(fd);
    pid_t left = fork();

    if (left > 0)
    {
        pid_t right = fork();
        if (right > 0)
        {
            close(fd[0]);
            close(fd[1]);

            int status_left = 0;
            int status_right = 0;

            waitpid(left, &status_left, 0);
            waitpid(right, &status_right, 0);

            return WEXITSTATUS(status_right);
        }
        else
        {
            close(fd[1]);
            dup2(fd[0], 0);
            close(fd[0]);
            //printf("right : \n");
            struct ast separator = { ast->type, ast->data, ast->nb_children,
                                    ast->child->node->child->next };
            //separator->child = ast->child->node->child->next->node;

            exit(eval_ast(&separator));
        }
    }
    else
    {
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        //printf("left : \n");

        struct ast separator = { ast->type, ast->data, ast->nb_children,
                                ast->child->node->child };
        //separator->child->child->node->child->node;

        exit(eval_ast(&separator/*ast->child->node*/));
    }

    return 0;
}

int eval_or(struct ast *ast)
{
    struct ast separator_left = { ast->type, ast->data, ast->nb_children,
                                ast->child->node->child };
    if (eval_ast(&separator_left))
    {
        struct ast separator_right = { ast->type, ast->data, ast->nb_children,
                                    ast->child->node->child->next };
        return eval_ast(&separator_right);
    }
    return 0;
}

int eval_and(struct ast *ast)
{
    struct ast separator_left = { ast->type, ast->data, ast->nb_children,
                                ast->child->node->child };
    int out = 0;
    if (!(out = eval_ast(&separator_left)))
    {
        struct ast separator_right = { ast->type, ast->data, ast->nb_children,
                                    ast->child->node->child->next };
        return eval_ast(&separator_right);
    }
    return out;
}

int eval_function(struct ast *ast)
{
    struct function *fun = function_list;
    char *ast_name = (char*) ast->data;
    while (strcmp(fun->name, ast_name) != 0)
        fun = fun->next;
    eval_children(fun->ast);
    return 0;
}

int choose_builtin(struct ast *ast)
{
    if (strcmp(ast->data, "shopt") == 0)
        return eval_shopt(ast);
    if (strcmp(ast->data, "exit") == 0)
        return eval_exit(ast);
    if (strcmp(ast->data, "cd") == 0)
        return eval_cd(ast);
    if (strcmp(ast->data, "export") == 0)
        return eval_export(ast);
    if (strcmp(ast->data, "echo") == 0)
        return eval_echo(ast);
    /*if (strcmp(ast->data, "continue") == 0)
        return eval_continue(ast);
    if (strcmp(ast->data, "break") == 0)
        return eval_break(ast);
    if (strcmp(ast->data, "source") == 0)
        return eval_source(ast);
    */
    else
        return 0;
}
int eval_operator(struct ast *ast)
{
    char *operator = ast->data;
    if (strcmp(operator, "=") == 0)
    {
        char *name = ast->child->node->data;
        char *value = ast->child->next->node->data;
        variable_update(name,value);
    }
    return 0;
}
int eval_expand(struct ast *ast)
{
    char *name_to_expand = ast->data;
    char *new_value = active_substitution(name_to_expand);
    variable_update(name_to_expand, new_value);
    //lexe le putain d'ast avec new_value then parse from ast;
    lexe(new_value);
    parse2(ast);
    struct ast *tmp = ast->child->node;
    ast->data = tmp->data;
    ast->child = tmp->child;
    tmp->child = NULL;
    free_ast(tmp);
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
        case T_FOR:
            return eval_for(ast);
        case T_CASE:
            return eval_case(ast);
        case T_UNTIL:
            return eval_until(ast);
        case T_FUNCTION_NAME:
            return eval_function(ast);
        case T_BUILTIN:
            return choose_builtin(ast);
        case T_OPERATOR:
            return eval_operator(ast);
        case T_EXPAND:
            return eval_expand(ast);
        default:
            return 0;
        }
    }
    return 0;
}
