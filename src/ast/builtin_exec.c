#define _GNU_SOURCE
#include <unistd.h>
#include "../include/global.h"
#include "header/astconvert.h"
#include "header/stringutils.h"
#include "../prompt/header/prompt.h"
#include "header/builtin_exec.h"
#include "../substitution/header/assignement_variables.h"
#include "../auxiliary/header/auxiliary.h"
#include "../lexer/header/syntax.h"

int last_return_value;
struct variables *variables;
char **environ;


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
            else if (*(tmp + i) == 'q')
                q = 1;
            else if (*(tmp + i) == 'u')
                u = 1;
            else
            {
                fprintf(stderr,"-%c is not a valid opt", *(tmp + i));
                return 0;
            }
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

/*!
 **  This function reproduces the atoi function.
 **  \param str : the string we want to convert into an integer.
 **  \return the integer stored in str.
 */
int my_atoi(const char *str)
{
    int i = 0;
    int result = 0;

    while (str[i] == ' '  && str[i] != '\0')
    {
        i += 1;
    }

    int is_neg = 0;
    int is_valid = 0;

    if (str[i] == '-')
    {
        is_neg = 1;
        i += 1;
    }

    if (str[i] == '+')
    {
        if (is_neg == 1)
            return 0;
        else
            i++;
    }

    while (str[i] != '\0' && is_valid == 0)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            result = result * 10 + str[i] - '0';
        }
        else
        {
            is_valid = 1;
        }
        i += 1;
    }

    if (is_valid == 1)
    {
        return -1;
    }

    return (is_neg) ? -result : result;
}

/*!
 **  This function checks if the input is the exit command.
 **  \param input : the string to be checked.
 **  \return 1 if the input string is the exit command, 0 otherwise.
 */
int is_exit(char *input)
{
    size_t len = strlen(input);
    size_t tmp = 0;
    if (tmp >= len - 2 || input[tmp] != 'e' || input[tmp + 1] != 'x'
            || input[tmp + 2] != 'i' || input[tmp + 3] !='t')
    {
        return 0;
    }
    return 1;
}


/*!
 **  This function reproduces the behaviours of the exit command.
 **  \param ast : the ast containing the parameters of the command.
 **  \return the value passed as parameter is it has one, 0 otherwise, and 2
 **  if the command has to many arguments.
 */
int eval_exit(struct ast *ast)
{
    size_t size = nb_nodes(ast);
    if (size == 0)
    {
        if (last_return_value)
        {
            return last_return_value;
        }
        else
        {
            return 0;
        }
    }
    else if (size > 1)
    {
        if (my_atoi(ast->child->node->data) < 0)
        {
            printf("Numerical argument is needed.\n");
            last_return_value = 2;
            return 2;
        }
        printf("too many arguments\n");
        last_return_value = 1;
        return 1;
    }
    else
    {
        int val = my_atoi(ast->child->node->data);
        if (val >= 0 && val <= 255)
        {
            last_return_value = val;
            return val;
        }
        else
        {
            printf("Numerical argument is needed.\n");
            last_return_value = 2;
            return 2;
        }
    }
}


/*!
 **  This function counts the number of nodes that ast contains.
 **  \param ast : the ast data structure we want to the length.
 **  \return The number of nodes in ast.
 */
size_t nb_nodes(struct ast *ast)
{
    size_t i = 0;
    struct node_list *tmp = ast->child;
    while (tmp)
    {
        i++;
        tmp = tmp->next;
    }

    return i;
}


/*!
 **  This function reproduces the behaviours of the cd command.
 **  \param ast : the ast containing the parameters of the command.
 **  \return 0 if the command is a success, 1 otherwise.
 */
int eval_cd(struct ast *ast)
{
    size_t size = nb_nodes(ast);
    if (size > 1)
    {
        printf("cd : too many arguments.\n");
        return 1;
    }

    if (size == 0)
    {
        char *home = getenv("HOME");
        char *tmp = getcwd(NULL, 0);

        if (chdir(home))
        {
            return 1;
        }

        variable_update("OLDPATH", tmp);

        free(tmp);
        return 1;
    }

    if (strcmp(ast->child->node->data, "-") == 0)
    {
        char *v = variable_value("OLDPATH");
        if (v != NULL)
        {
            char *tmp = getcwd(NULL, 0);

            if (chdir(v))
            {
                return 1;
            }

            variable_update("OLDPATH", tmp);

            free(tmp);
            return 0;
        }
        else
        {
            printf("OLDPATH not defined.\n");
            return 1;
        }
    }


    char *tmp = getcwd(NULL, 0);
    if (chdir(ast->child->node->data))
    {
        return 1;
    }

    variable_update("OLDPATH", tmp);

    free(tmp);
    return 0;
}


/*!
 **  This function prints the input string following the -E echo option.
 **  \param m : the input string we want to display.
 **  \param len : the length of the input string.
 */
void print_E_op(char *m, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (m[i] == '\\')
        {
            putchar('\\');
        }
        else if (m[i] == '\a')
        {
            putchar('\\');
            putchar('a');
        }
        else if (m[i] == '\b')
        {
            putchar('\\');
            putchar('b');
        }
        else if (m[i] == '\f')
        {
            putchar('\\');
            putchar('f');
        }
        else if (m[i] == '\n')
        {
            putchar('\\');
            putchar('n');
        }
        else if (m[i] == '\r')
        {
            putchar('\\');
            putchar('r');
        }
        else if (m[i] == '\t')
        {
            putchar('\\');
            putchar('t');
        }
        else if (m[i] == '\v')
        {
            putchar('\\');
            putchar('v');
        }
        else
        {
            putchar(m[i]);
        }
    }
}


/*!
 **  This function prints the input string followint the -e echo option.
 **  \param m : the input string we want to display.
 **  \param len : the length of the input string.
 */
void print_e_op(char *m, size_t len)
{
    int b_slash = 0;

    for (size_t i = 0; i < len; i++)
    {
        if (m[i] == '\\' && b_slash == 0)
        {
            b_slash = 1;
        }
        if (m[i] == '\\' && b_slash == 1)
        {
            b_slash = 0;
            putchar('\\');
        }
        else if (m[i] == '\a' && b_slash == 0)
        {
            b_slash = 0;
            putchar('\a');
        }
        else if (m[i] == '\b' && b_slash == 0)
        {
            b_slash = 0;
            putchar('\b');
        }
        else if (m[i] == '\f' && b_slash == 0)
        {
            b_slash = 0;
            putchar('\f');
        }
        else if (m[i] == '\n' && b_slash == 0)
        {
            b_slash = 0;
            putchar('\n');
        }
        else if (m[i] == '\r' && b_slash == 0)
        {
            b_slash = 0;
            putchar('\r');
        }
        else if (m[i] == '\t' && b_slash == 0)
        {
            b_slash = 0;
            putchar('\t');
        }
        else if (m[i] == '\v' && b_slash == 0)
        {
            b_slash = 0;
            putchar('\v');
        }
        else
        {
            putchar(m[i]);
        }
    }
}

/*!
 **  This function checks if the input is an echo option or not.
 **  \param input : the input strint to be checked.
 **  \return A value greater than 0 if the input is an echo option, 0 otherwise.
 */
int is_option(char *input)
{
    if (strcmp(input, "-n") == 0)
    {
        return 1;
    }
    else if (strcmp(input, "-e") == 0)
    {
        return 2;
    }
    else if (strcmp(input, "-E") == 0)
    {
        return 3;
    }
    else
    {
        return 0;
    }
}


/*!
 **  This function checks if the ast only contains echo options.
 **  \param ast : the ast to be checked.
 **  \return 1 is the ast only contains echo options, 0 otherwise.
 */
int is_full_options(struct ast *ast)
{
    size_t size = nb_nodes(ast);
    size_t i = 0;
    struct node_list *tmp = ast->child;
    while (i < size && is_option(tmp->node->data) > 0)
    {
        i++;
        tmp = tmp->next;
    }

    return i == size;
}


/*!
 **  This function follows the behaviour of echo, called with only options.
 **  \param ast : the ast containing the command echo with only options.
 */
void print_full_options(struct ast *ast)
{
    int n_op = 0;

    struct node_list *tmp = ast->child;
    int op;
    while (tmp && (op = is_option(tmp->node->data)) > 0)
    {
        if (op == 1)
        {
            n_op = 1;
        }

        tmp = tmp->next;
    }

    if (n_op == 0)
    {
        printf("\n");
    }
}


/*!
 **  This function reproduces the behaviours of the echo command.
 **  \param ast : the ast containing the echo parameters.
 **  \return 0 at the end of the function, such as echo.
 */
int eval_echo(struct ast *ast)
{
    size_t n_op = 0;
    size_t e_op = 0;

    size_t size = nb_nodes(ast);
    if (!ast->child)
    {
        printf("\n");
        return 0;
    }
    else if (size == 1)
    {
        if (is_option(ast->child->node->data) == 1)
        {
            return 0;
        }
        if (is_option(ast->child->node->data) == 2
                || is_option(ast->child->node->data) == 3)
        {
            putchar('\n');
            return 0;
        }
    }
    else if (is_full_options(ast) == 1)
    {
        print_full_options(ast);
        return 0;
    }
    struct node_list *tmp = ast->child;
    int op;
    while (tmp && (op = is_option(tmp->node->data)) > 0)
    {
        if (op == 1)
        {
            n_op = 1;
        }
        else if (op == 2)
        {
            e_op = 1;
        }

        tmp = tmp->next;
    }

    if (e_op == 1)
    {
        while (tmp->next)
        {
            print_e_op(tmp->node->data, strlen(tmp->node->data));
            printf(" ");
            tmp = tmp->next;
        }
        print_e_op(tmp->node->data, strlen(tmp->node->data));
        if (n_op == 0)
        {
            putchar('\n');
        }
    }
    else
    {
        while (tmp->next)
        {
            print_E_op(tmp->node->data, strlen(tmp->node->data));
            printf(" ");
            tmp = tmp->next;
        }
        print_E_op(tmp->node->data, strlen(tmp->node->data));
        if (n_op == 0)
        {
            printf("\n");
        }
    }

    return 0;
}


/*!
 **  This function returns the name of the variable in the input string.
 **  \param input : the string containing the variable.
 **  \param index : the current index in the input string.
 **  \param len : the length of the input string.
 **  \return The string containing only the name of the variable.
 */
char *get_var_name(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    while (input[tmp] != '=' && tmp < len)
    {
        tmp += 1;
    }
    char *name = cut(input, index, tmp, len);
    *index = tmp + 1;
    return name;
}


/*!
 **  This function returns the value of the variable in the input string.
 **  \param input : the string containing the variable.
 **  \param index : the current index in the input string.
 **  \param len : the length of the input string.
 **  \return The string containing only the value of the variable.
 */
char *get_var_value(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    while (input[tmp] != '\0' && tmp < len)
    {
        tmp += 1;
    }
    char *value = cut(input, index, tmp, len);
    return value;
}


/*!
 **  This function reproduces the behaviours of the export command.
 **  \param ast : the ast containing the arguments of the command.
 **  \return 0 if the variable(s) could be exported, 1 if it failed.
 */
int eval_export(struct ast *ast)
{
    size_t size = nb_nodes(ast);

    if (size == 0)
    {
        int i = 0;
        while(environ[i])
        {
            printf("declare -x %s\n", environ[i++]);
        }
        print_variables();
        return 0;
    }
    else if (size == 1 && strcmp(ast->child->node->data, "-p") == 0)
    {
        int i = 0;
        while(environ[i])
        {
            printf("declare -x %s\n", environ[i++]);
        }
        print_variables();
        return 0;
    }
    else
    {
        size_t j = 0;
        struct node_list *tmp = ast->child;
        while (j < size)
        {
            size_t i = 0;
            size_t len = strlen(ast->child->node->data);
            char *name = get_var_name(tmp->node->data, &i, len);

            char *value = get_var_value(tmp->node->data, &i, len);

            variable_update(name, value);
            putenv(ast->child->node->data);

            j++;
            tmp = tmp->next;
            free(name);
            free(value);
        }

        return 0;
    }

    return 0;
}


/*!
**  This function reproduces the behaviour of the continue command in case
**  of error.
**  \param ast : the ast containing the parameters of the command.
**  \return 0, as the continue command.
*/
int eval_continue(struct ast *ast)
{
    ast = ast;
    printf("continue : only has a meaning in a 'for',");
    printf("'while' or 'until' loop.\n");
    return 0;
}


/*!
**  This function reproduces the behaviour of the break command in case
**  of error.
**  \param ast : the ast containing the parameters of the command.
**  \return 0, as the break command.
*/
int eval_break(struct ast *ast)
{
    ast = ast;
    printf("break : only has a meaning in a 'for',");
    printf("'while' or 'until' loop.\n");
    return 0;
}


/*!
**  This function reproduces the behaviours of the source command.
**  \param ast : the ast containing the parameters of the command.
**  \return 0 as the source command.
*/
int eval_source(struct ast *ast)
{
    ast = ast;
    return 0;
}
