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
    if (ast->child->node->type == T_SEPARATOR)
        return eval_command(ast->child->node);

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

    size_t len = 0;
    void *copy = strdup(ast->child->node->data);
    char **command = cut_line(copy, &len);
    int out = execution(command, command[0]);
    //printf("Return value of |%s|: %d\n", command[0], out);
    free(copy);
    free(command);
    copy = NULL;
    command = NULL;
    return out;
}

/*!
**  Evaluates all the conditions of a if/while node
**  \return The return value of the last exec
**/
int eval_conditions(struct ast *ast)
{
    int out = 0;
    struct node_list *conditions = ast->child;

    while (conditions->node->type == T_SEPARATOR)
    {
        out = eval_command(conditions->node);
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


int printer_shopt(int setted)
{
    for (int i = 0; i < 8; i++)
    {
        if (setted == 0)
        {
            if (shopt_opt_nbr[i] == 0)
                printf("%s  off\n", shopt_opt[i]);
            else
                printf("%s  on\n", shopt_opt[i]);
        }
        if (setted == 1)
        {
            if (shopt_opt_nbr[i] == 0)
                printf("%s  off\n", shopt_opt[i]);
        }
        if (setted == 2)
        {
            if (shopt_opt_nbr[i] == 1)
                printf("%s  on\n", shopt_opt[i]);
        }
    }
    return 0;
}
int un_set_shopt(struct node_list *curr, int setter)
{
    int result = 0;
    while (curr)
    {
        int passed = 0;
        for (int i = 0; i < 8; i++)
        {
            if (strcmp((char*) curr->node->data, shopt_opt[i]) == 0)
            {
                shopt_opt_nbr[i] = setter;
                passed = 1;
            }
        }
        if (passed == 0)
        {
            fprintf(stderr, "%s : invalid shell opt namei\n",
                    (char*)curr->node->data);
            result = 1;
        }
        curr = curr->next;
    }
    return result;
}
int checker_shopt(struct node_list *curr)
{
    int passed = 0;
    while (curr)
    {
        for (int i = 0; i < 8; i++)
        {
            if (strcmp((char*) curr->node->data, shopt_opt[i]) == 0)
            {
                if (shopt_opt_nbr[i] == 0)
                passed = 1;
            }
        }

    }
    return passed;
}

int eval_shopt(struct ast *ast)
{
    if (!ast->child)
    {
        printer_shopt(0);
        return 0;
    }
    int s = 0;
    int q = 0;
    int u = 0;
    struct node_list *curr = ast->child;
    char *tmp = curr->node->data;
    while(curr && *(tmp) == '-')
    {
        tmp = curr->node->data;
        for (int i = 1; *(tmp + i) != '\0'; i++)
        {
            if (*(tmp+ i) == 's')
                s = 1;
            if (*(tmp + i) == 'q')
                q = 1;
            if (*(tmp + i) == 'u')
                u = 1;
        }
        curr = curr->next;
    }
    if (s == 1 && u == 1)
    {
        fprintf(stderr,"can't set and unset an shopt opt\n");
        return 0;
    }
    if (s == 1 && q == 0 && curr->next == NULL)
        return printer_shopt(2);
    if (u == 1 && q == 0 && curr->next == NULL)
        return printer_shopt(1);
    if (q == 1 && curr->next == NULL)
        return 0;
    if (s == 1 && q == 0)
        return un_set_shopt(curr, 1);
    if (u == 1 && q == 0)
        return un_set_shopt(curr, 0);
    if (q == 1)
        return checker_shopt(curr);
    return 0;


}

int choose_builtin(struct ast *ast)
{
    if (strcmp(ast->data, "shopt") == 0)
        return eval_shopt(ast);
    else
        return 0;
}

//Left part is the beginning of the command
//So what needs to be done is get the right part
//Save current state of lexer
//Save the current state of stdin, dup2 stdout in stdin
//relexe parse eval, empty the right part
//Restore lexer
//restore stdout
//close fd where stdout was saved
int eval_expand(struct ast *ast)
{
    if (ast)
        return 0;

    struct token *lexer_save = lexer->head;
    lexer->head = NULL;

    int save_stdin = dup(0);

    dup2(1, 0);//duplicates stdout into stdin

    //char *expand_content = (ast->child->next) ?
    //                          (char*) ast->child->next->data :
    //                          (char*) ast->child->data;
    //
    //lexe then parse content of the right node

    dup2(save_stdin, 0);
    close(save_stdin);
    lexer->head = lexer_save;
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
        case T_NONE:                    //temporary til right type is created
            return eval_expand(ast);
        default:
            return 0;
        }
    }
    return 0;
}
