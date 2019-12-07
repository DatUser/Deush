#define _GNU_SOURCE
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
#include "header/loop.h"
#include "header/stringutils.h"
#include "../prompt/header/prompt.h"
#include "header/builtin_exec.h"
#include "../substitution/header/assignement_variables.h"



char *shopt_opt[8] = {"ast_print", "dotglob", "expand_aliases","extglob",
                           "nocaseglob", "nullglob", "sourcepath", "xpg_echo"};

int shopt_opt_nbr[8] = {0, 0, 0, 0, 0, 0, 0, 0};

struct env env = { 0, -1, -1, -1 };

int eval_script(struct ast *ast)
{
    /*char *cmd = strdup(ast->child->node->data);
    cmd = pack_command(ast->child->node->child, cmd); pour filimon
    char **command = cut_line(cmd, &len);*/

    if (access(ast->data, F_OK) < 0)
    {
        warnx("No such file or directory");
        return 127;
    }
    else if (access(ast->data, X_OK) < 0 || access(ast->data, R_OK))
    {
        warnx("Permission denied");
        return 126;
    }
    else
    {
        FILE *file = fopen(ast->data, "r");

        begin_script(ast);

        run_script(/*save, fd*/file);
        script_del_args();

        //create_ast_file(ast);
        //exit(0);

        return last_return_value;
    }
}

void cancel_type(void)
{
    struct token *tmp = lexer->head;

    while (tmp && tmp->next && tmp->next->primary_type != T_NEWLINE)
    {
        if (tmp->primary_type != T_EXPAND && tmp->primary_type != T_COMMANDSUB)
            tmp->primary_type = T_WORD;
        tmp = tmp->next;
    }
    if (tmp && tmp->next)
    {
        if (tmp->primary_type != T_EXPAND && tmp->primary_type != T_COMMANDSUB)
            tmp->primary_type = T_WORD;
        free(tmp->next->value);
        free(tmp->next);
        tmp->next = NULL;
    }

}

void free_children(struct ast *ast)
{
    struct node_list *children = ast->child;

    while (children)
    {
        struct node_list *next = children->next;
        free_ast(children->node);
        free(children);
        children = next;
    }
    ast->child = NULL;
}

void rearrange_node(struct ast *ast)
{
    void *cmd = strdup(ast->data);
    cmd = pack_command(ast->child, cmd);

    struct token *save = lexer->head;
    lexer->head = NULL;

    lexe(cmd);
    cancel_type();

    free(ast->data);
    ast->data = (lexer->head) ? lexer->head->value : strdup("");
    struct token *head = pop_lexer();
    free(head);

    free_children(ast);

    parse_wordlist(&ast);

    lexer = re_init_lexer(lexer);
    lexer->head = save;

    free(cmd);
}

int eval_operator_redirection(struct ast *ast, int *evaluated)
{
    int changed = 0;//this is useless
    eval_command_substitution(ast->child->node);
    eval_expand(ast->child->node, &changed);

    if (changed)
        rearrange_node(/*ast->child->node->child->node,*/ ast->child->node);

    char *separator = ast->child->node->data;

    if (!separator)
        return last_return_value;

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
    if (ast->child->node->type == T_BUILTIN)
        return choose_builtin(ast->child->node);

    *evaluated = 0;
    return 0;
}

char *pack_command(struct node_list *children, char *cmd)
{
    struct node_list *tmp = children;

    while (tmp)
    {
        char *arg = tmp->node->data;
        cmd = str_concat_space(cmd, arg);
        tmp = tmp->next;
    }

    return cmd;
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
    char *check = ast->child->node->data;

    if (check && check[0])
    {
        void *cmd = strdup(ast->child->node->data);
        cmd = pack_command(ast->child->node->child, cmd);
        char **command = cut_line(cmd, &len);
        int out = execution(command, command[0]);
        //printf("Return value of |%s|: %d\n", command[0], out);
        free(cmd);
        free(command);
        cmd = NULL;
        command = NULL;
        return out;
    }

    return 0;
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

int continue_loop(struct ast *ast, int is_continue)
{
    char *nb = ast->data;
    if (is_continue)
        env.is_continue = 1;
    else
        env.is_break = 1;
    int loop =  atoi(nb);//extract_nb(nb);
    //printf("arg_depth is : %d\nenv_depht is : %d\n", loop, env.depth);
    if (loop > 0 && is_num(nb))
        env.break_until = (loop >= env.depth) ? 1 : (env.depth - loop + 1);
    else
    {
        warnx("Illegal number %s", nb);
        env.break_until = 1;
        return 128;
    }
    return 0;
}

/*!
**  Evaluates all the children of a node while/for
**/
int eval_children_loop(struct ast *ast)
{
    struct node_list *tmp = ast->child;
    char *command = NULL;
    int error = 0;

    while (tmp && strcmp((command = tmp->node->child->node->data), "continue")
        && strcmp(command, "break")
        && env.is_continue < 0
        && (env.is_break < 0 || env.break_until > env.depth))
    {
        error = eval_ast(tmp->node);
        command = tmp->node->child->node->data;
        tmp = tmp->next;
    }

    int is_continue = !strcmp(command, "continue");
    int is_break = (is_continue) ? 0 : !strcmp(command, "break");
    if (tmp && (is_continue || is_break))
    {
        if (tmp->node->child->node->child)
            error = (is_continue) ?
            continue_loop(tmp->node->child->node->child->node, 1)
            : continue_loop(tmp->node->child->node->child->node, 0);
        else if (is_break)
        {
            env.break_until = env.depth;
            env.is_break = 1;
        }
    }

    return error;
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
    while (tmp && j != *i)
    {
        j++;
        tmp = tmp->next;
    }
    while (tmp && tmp->node->type != type)
    {
        j++;
        tmp = tmp->next;
    }
    if (tmp)
    {
        *i = j + 1;
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
            int j = 0;
            struct ast *then_node = find_node(elif_node->child, T_THEN, &j);
            elif_pa = 1;
            return eval_children(then_node);//eval_ast(elif_node->child->node);
        }
    }
    struct ast *else_node = find_node(ast->child, T_ELSE, &i);
    if (else_node)
        return eval_children(else_node);//eval_ast(else_node->child->node);
    return 0;
}

/*!
**  Resets the environment
**/
void reset_env(void)
{
    if (env.is_continue > 0 && env.break_until == env.depth)
    {
        env.break_until = -1;
        env.is_continue = -1;
    }
    else if (env.depth == 0)
    {
        env.is_break = -1;
        env.break_until = -1;
    }
}

/*!
**  Evaluates a node that i of type while
**  \param ast : Node of type while
**  \return The return value is 0 by default
**/
int eval_while(struct ast *ast)
{
    env.depth += 1;

    int i = 0;
    int error = 0;
    //struct ast *condition_node = find_node(ast->child, T_SEPARATOR, &i);
    struct ast *do_node = find_node(ast->child, T_DO, &i);
    while (!eval_conditions(ast))
    {
        //reset_env();
        error = eval_children_loop(do_node);
        reset_env();
    }

    env.depth -= 1;

    return error;
}

/*!
**  Evaluates a node that i of type while
**  \param ast : Node of type while
**  \return The return value is 0 by default
**/
int eval_until(struct ast *ast)
{
    env.depth += 1;

    int i = 0;
    int error = 0;
    //struct ast *condition_node = find_node(ast->child, T_SEPARATOR, &i);
    struct ast *do_node = find_node(ast->child, T_DO, &i);
    while (eval_conditions(ast))
    {
        //reset_env();
        error = eval_children_loop(do_node);
        reset_env();
    }

    env.depth -= 1;

    return error;
}

void expansion(struct ast *ast)
{
    int changed = 0;//this is useless
    eval_command_substitution(ast);
    eval_expand(ast, &changed);

    if (changed)
        rearrange_node(/*ast->child->node,*/ ast);
}

/*!
**  Evaluates a node that is of type for
**  \param ast : Node of type for
**  \return The return value is 0 by default
**/
int eval_for(struct ast *ast)
{
    env.depth += 1;

    int i = 0;
    struct ast *in_node = find_node(ast->child, T_IN, &i);
    struct ast *do_node = find_node(ast->child, T_DO, &i);

    expansion(in_node);

    struct node_list *tmp = in_node->child;
    int error = 0;

    while (tmp)
    {
        //reset_env();
        if (env.break_until < 0)
            error = eval_children_loop(do_node);
        reset_env();
        tmp = tmp->next;
    }

    env.depth -= 1;

    return error;
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
    {
        char *s = malloc(sizeof(char) * 50);
        if (s == NULL)
        {
            return 1;
        }
        s = my_itoa(last_return_value, s);

        last_return_value = eval_shopt(ast);
        variable_update("?", s);
        free(s);
        return last_return_value;
    }
    if (strcmp(ast->data, "exit") == 0)
    {
        char *s = malloc(sizeof(char) * 50);
        if (s == NULL)
        {
            return 1;
        }
        s = my_itoa(last_return_value, s);
        last_return_value = eval_exit(ast);
        variable_update("?", s);
        free(s);
        return last_return_value;
    }
    if (strcmp(ast->data, "cd") == 0)
    {
        char *s = malloc(sizeof(char) * 50);
        if (s == NULL)
        {
            return 1;
        }
        s = my_itoa(last_return_value, s);

        last_return_value = eval_cd(ast);
        variable_update("?", s);
        free(s);
        return last_return_value;
    }
    if (strcmp(ast->data, "export") == 0)
    {
        char *s = malloc(sizeof(char) * 50);
        if (s == NULL)
        {
            return 1;
        }
        s = my_itoa(last_return_value, s);

        last_return_value = eval_export(ast);
        variable_update("?", s);
        free(s);
        return last_return_value;
    }
    if (strcmp(ast->data, "echo") == 0)
    {
        char *s = malloc(sizeof(char) * 50);
        if (s == NULL)
        {
            return 1;
        }
        s = my_itoa(last_return_value, s);

        last_return_value = eval_echo(ast);
        variable_update("?", s);
        free(s);
        return last_return_value;
    }
    if (strcmp(ast->data, "continue") == 0)
    {
        char *s = malloc(sizeof(char) * 50);
        if (s == NULL)
        {
            return 1;
        }
        s = my_itoa(last_return_value, s);

        last_return_value = eval_continue(ast);
        variable_update("?", s);
        free(s);
        return last_return_value;
    }
    if (strcmp(ast->data, "break") == 0)
    {
        char *s = malloc(sizeof(char) * 50);
        if (s == NULL)
        {
            return 1;
        }
        s = my_itoa(last_return_value, s);

        last_return_value = eval_break(ast);
        variable_update("?", s);
        free(s);
        return last_return_value;
    }
    if (strcmp(ast->data, "source") == 0)
    {
        char *s = malloc(sizeof(char) * 50);
        if (s == NULL)
        {
            return 1;
        }
        s = my_itoa(last_return_value, s);

        last_return_value = eval_source(ast);
        variable_update("?", s);
        free(s);
        return last_return_value;
    }
    if (strcmp(ast->data, ".") == 0)
    {
        char *s = malloc(sizeof(char) * 50);
        if (s == NULL)
        {
            return 1;
        }
        s = my_itoa(last_return_value, s);

        last_return_value = eval_source(ast);
        variable_update("?", s);
        free(s);
        return last_return_value;
    }
    if (strcmp(ast->data, "alias") == 0)
    {
        char *s = malloc(sizeof(char) * 50);
        if (s == NULL)
        {
            return 1;
        }
        s = my_itoa(last_return_value, s);

        last_return_value = eval_alias(ast);
        variable_update("?", s);
        free(s);
        return last_return_value;
    }
    if (strcmp(ast->data, "unalias") == 0)
    {
        char *s = malloc(sizeof(char) * 50);
        if (s == NULL)
        {
            return 1;
        }
        s = my_itoa(last_return_value, s);

        last_return_value = eval_unalias(ast);
        variable_update("?", s);
        free(s);
        return last_return_value;
    }
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
        add_variable(name,value);
        eval_children(ast);
    }
    return 0;
}

//Needs to read stdout from tmp
//pack it into 1 node then run the command
//Fais comme eval_expand
void substitute_command(struct ast *ast)
{
    int tmp = open("/tmp", O_TMPFILE|O_RDWR);

    struct token *lexer_save = lexer->head;
    lexer->head = NULL;

    int save_stdout = dup(1);

    dup2(tmp, 1);//duplicates tmp file descriptor into stdout

    run_command_sub(ast->data);

    free(ast->data);
    ast->data = extract_file(tmp);

    dup2(save_stdout, 1);
    close(save_stdout);
    close(tmp);
    lexer->head = lexer_save;
}

void eval_command_substitution(struct ast *ast)
{
    if (ast)
    {
        if (ast->type == T_COMMANDSUB)
        {
            substitute_command(ast);
            ast->type = T_WORD;
        }

        struct node_list *tmp = ast->child;

        while (tmp)
        {
            if (tmp->node->type == T_COMMANDSUB)
            {
                substitute_command(tmp->node);
                tmp->node->type = T_WORD;
            }
            tmp = tmp->next;
       }
    }
}

int eval_expand(struct ast *ast, int *changed)
{
    if (ast)
    {
        char *name_to_expand = NULL;
        char *new_value =  NULL;
        char *stock = NULL;
        if (ast->type == T_EXPAND)
        {
            name_to_expand = ast->data;
            new_value = active_substitution(name_to_expand);
            name_to_expand = delete_expansion(name_to_expand);
            variable_update(name_to_expand, new_value);
            stock = ast->data;
            ast->data = strdup(new_value);
            free(stock);
            ast->type = T_WORD;
            *changed = 1;
        }
        if (ast->child)
        {
            struct node_list *tmp = ast->child;
            while (tmp)
            {
                if (tmp->node->type == T_EXPAND)
                {
                    name_to_expand = tmp->node->data;
                    new_value = active_substitution(name_to_expand);
                    variable_update(delete_expansion(name_to_expand),
                        new_value);
                    stock = tmp->node->data;
                    tmp->node->data = strdup(new_value);
                    free(stock);
                    tmp->node->type = T_WORD;
                    *changed = 1;
                }
                tmp = tmp->next;
            }
        }
    }
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
        reset_env();
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
        case T_SCRIPT:
            return eval_script(ast);
        /*case T_EXPAND:
            return eval_expand(ast);
        case T_WORD:
            return 0;*/
        default:
            return 0;
        }
    }
    return 0;
}
