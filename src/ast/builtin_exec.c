#define _GNU_SOURCE
#include <unistd.h>
#include "../include/global.h"
#include "header/astconvert.h"
#include "header/stringutils.h"
#include "../prompt/header/prompt.h"
#include "header/builtin_exec.h"
#include "../substitution/header/assignement_variables.h"
#include "../auxiliary/header/auxiliary.h"

int last_return_value;
struct variables *variables;
char **environ;
struct aliases *aliases;


/*!
**  This function prints the shopt options.
**  \param setted : an integer that indicates if the shopt option is setted
**  or not.
**  \return 0 always.
*/
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


/*!
**  This function sets or unsets a shopt option.
**  \param curr : the node_list that contains the name of the option.
**  \param setter : the integer that sets or unsets the option.
**  \return 0 if the option could be set or unset, 1 otherwise.
*/
int un_set_shopt(struct node_list *curr, int setter)
{
    int result = 0;
    while (curr)
    {
        int passed = 0;
        for (int i = 0; i < 8; i++)
        {
            if (strcmp(curr->node->data, shopt_opt[i]) == 0)
            {
                shopt_opt_nbr[i] = setter;
                passed = 1;
            }
        }
        if (passed == 0)
        {
            char *s = curr->node->data;
            fprintf(stderr, "%s : invalid shell opt namei\n", s);
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
            if (strcmp(curr->node->data, shopt_opt[i]) == 0)
            {
                if (shopt_opt_nbr[i] == 0)
                    passed = 1;
            }
        }

    }
    return passed;
}

/*!
**  This function checks if the input is valid shopt option.
**  \param input : the input string to be checked.
**  \return 1 if the input string if a valid shopt option, 0 otherwise.
*/
int is_shopt_option(char *input)
{
    if (strcmp(input, "-s") == 0)
    {
        return 1;
    }
    else if (strcmp(input, "-q") == 0)
    {
        return 2;
    }
    else if (strcmp(input, "-u") == 0)
    {
        return 3;
    }
    else if (strcmp(input,"-sq") == 0 || strcmp(input, "-qs") == 0)
    {
        return 4;
    }
    else if (strcmp(input, "-qu") == 0 || strcmp(input, "-uq") == 0)
    {
        return 5;
    }
    else if (strcmp(input, "-su") == 0 || strcmp(input, "-us") == 0)
    {
        return 6;
    }
    else if (strcmp(input, "-squ") == 0 || strcmp(input, "-suq") == 0
        || strcmp(input, "-qsu") == 0 || strcmp(input, "-qus") == 0
        || strcmp(input, "usq") == 0 || strcmp(input, "-uqs") == 0)
    {
        return 7;
    }
    else
    {
        return 0;
    }
}


/*!
**  This function reproduces the behaviours of the shopt command.
**  \param ast : the ast containing the parameters of the command.
**  \return The exact same values as the shopt command.
*/
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
    while (curr)
    {
        char *tmp = curr->node->data;
        if (is_shopt_option(tmp) == 1)
        {
            s = 1;
            curr = curr->next;
        }
        else if (is_shopt_option(tmp) == 2)
        {
            q = 1;
            curr = curr->next;
        }
        else if (is_shopt_option(tmp) == 3)
        {
            u = 1;
            curr = curr->next;
        }
        else if (is_shopt_option(tmp) == 4)
        {
            s = 1;
            q = 1;
            curr = curr->next;
        }
        else if (is_shopt_option(tmp) == 5)
        {
            u = 1;
            q = 1;
            curr = curr->next;
        }
        else if (is_shopt_option(tmp) == 6)
        {
            s = 1;
            u = 1;
            curr = curr->next;
        }
        else if (is_shopt_option(tmp) == 7)
        {
            s = 1;
            u = 1;
            q = 1;
            curr = curr->next;
        }
        else if (*(tmp) == '-')
        {
            fprintf(stderr,"%s is not a valid opt\n", tmp);
            return 1;
        }
        else
        {
            break;
        }
    }

    if (s == 1 && u == 1)
    {
        fprintf(stderr,"can't set and unset a shopt opt\n");
        return 0;
    }
    if (s == 1 && q == 0 && curr == NULL)
        return printer_shopt(2);
    if (u == 1 && q == 0 && curr == NULL)
        return printer_shopt(1);
    if (q == 1 && curr == NULL)
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
            fprintf(stderr, "Numerical argument is needed.\n");
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
 **  This function restores the home path.
 **  \param input : the home path to be restored.
 **  \return The restored home path.
 */
char *remove_path(char *input)
{
    size_t i = 0;
    while (input[i] != '\0' && input[i] != '.')
    {
        i++;
    }
    input[i] = '\0';
    return input;
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
        fprintf(stderr, "cd : too many arguments.\n");
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

        char *v = variable_value("OLDPWD");
        if (v != NULL)
        {
            variable_update("OLDPWD", tmp);
        }
        else
        {
            add_variable("OLDPWD", tmp);
        }

        free(tmp);
        return 0;
    }

    if (strcmp(ast->child->node->data, "-") == 0)
    {
        char *v = variable_value("OLDPWD");
        if (v != NULL)
        {
            char *tmp = getcwd(NULL, 0);

            if (chdir(v))
            {
                return 1;
            }

            variable_update("OLDPWD", tmp);

            free(tmp);
            return 0;
        }
        else
        {
            printf("OLDPWD not defined.\n");
            return 1;
        }
    }


    char *tmp = getcwd(NULL, 0);
    if (chdir(ast->child->node->data))
    {
        return 1;
    }

    char *v = variable_value("OLDPWD");
    if (v != NULL)
    {
        variable_update("OLDPWD", tmp);
    }
    else
    {
        add_variable("OLDPWD", tmp);
    }

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
            printf("export %s\n", environ[i++]);
        }
        print_variables();
        return 0;
    }
    else if (size == 1 && strcmp(ast->child->node->data, "-p") == 0)
    {
        int i = 0;
        while(environ[i])
        {
            printf("export %s\n", environ[i++]);
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
    //printf("continue : only has a meaning in a 'for',");
    //printf("'while' or 'until' loop.\n");
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
    //printf("break : only has a meaning in a 'for',");
    //printf("'while' or 'until' loop.\n");
    return 0;
}


/*!
 **  This function reproduces the behaviours of the source command.
 **  \param ast : the ast containing the parameters of the command.
 **  \return 0 as the source command.
 */
int eval_source(struct ast *ast)
{
    size_t size = nb_nodes(ast);
    if (size == 0)
    {
        printf("filename argument required.\n");
        return 1;
    }

    FILE *f = fopen(ast->child->node->data, "r");
    if (f == NULL)
    {
        printf("no such file of directory\n");
        return 1;
    }

    size_t len = 0;
    char *line = NULL;
    ssize_t read = getline(&line, &len, f);
    while (read != -1)
    {
        lexe(line);
        if (is_good_grammar())
        {
            printf("wrong grammar.\n");
            lexer = re_init_lexer(lexer);
            return 1;
        }
        parse2(NULL);
        read = getline(&line, &len, f);
    }
    free(line);
    return 0;
}


/*!
 **  This function initializes an alias.
 **  \param name : the name of the alias.
 **  \param value : the value of the alias.
 **  \return The alias if it could be created, NULL otherwise.
 */
struct aliases *init_alias(char *name, char *value)
{
    struct aliases *new = malloc(sizeof(struct aliases));
    if (new == NULL)
    {
        return NULL;
    }

    char *n = strdup(name);
    char *v = strdup(value);
    new->name = n;
    new->value = v;
    new->next = NULL;
    return new;
}


/*!
 **  This function adds an alias to the aliases data structure.
 **  \param alias : the alias to be added.
 */
void add_alias(struct aliases *alias)
{
    if (aliases == NULL)
    {
        aliases = alias;
        return;
    }

    struct aliases *tmp = aliases;
    while (tmp->next)
    {
        tmp = tmp->next;
    }
    tmp->next = alias;
}


/*!
 **  This function prints the aliases stored in the aliases data structure.
 */
void print_alias(void)
{
    struct aliases *tmp = aliases;
    while (tmp)
    {
        printf("alias %s='%s'\n", tmp->name, tmp->value);
        tmp = tmp->next;
    }
}


/*!
 **  This function frees all the aliases stored it the aliases data structure.
 */
void free_alias(void)
{
    if (aliases)
    {
        struct aliases *tmp = aliases;
        while (tmp)
        {
            struct aliases *t = tmp;
            tmp = tmp->next;
            free(t->name);
            free(t->value);
            free(t);
        }
        aliases = NULL;
    }
}


/*!
 **  This function checks if the input string has an equal character.
 **  \param input : the input string we want to check.
 **  \return 1 if the input string has an equal character, 0 otherwise.
 */
int has_equal_character(char *input)
{
    size_t i = 0;
    while (input[i])
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
 **  This function reproduces the behaviours of the alias command.
 **  \param : ast : the ast containing the parameters of the command.
 **  \return the exact same values as the alias command.
 */
int eval_alias(struct ast *ast)
{
    size_t size = nb_nodes(ast);
    if (size == 0)
    {
        print_alias();
        return 0;
    }

    size_t j = 0;
    struct node_list *tmp = ast->child;
    while (j < size)
    {
        if (has_equal_character(tmp->node->data) == 0)
        {
            struct aliases *t = find_alias(tmp->node->data);
            if (t)
            {
                printf("alias %s=%s\n", t->name, t->value);
                return 0;
            }
            else
            {
                char *s = tmp->node->data;
                printf("alias : %s not found\n", s);
                return 1;
            }
        }

        size_t i = 0;
        size_t len = strlen(tmp->node->data);
        char *name = get_var_name(tmp->node->data, &i, len);
        char *value = get_var_value(tmp->node->data, &i, len);

        struct aliases *new = init_alias(name, value);
        if (new != NULL)
        {
            add_alias(new);
        }
        j++;
        tmp = tmp->next;

        free(name);
        free(value);
    }

    return 0;
}


/*!
 **  This function checks if there is an alias with a specific name.
 **  \param name : the name of the alias we want to find.
 **  \return 1 if there an alias with the name 'name', 0 otherwise.
 */
struct aliases *find_alias(char *name)
{
    struct aliases *tmp = aliases;
    while (tmp)
    {
        if (strcmp(tmp->name, name) == 0)
        {
            return tmp;
        }

        tmp = tmp->next;
    }
    return NULL;
}


/*!
 **  This function deletes an alias with a specific name.
 **  \param name : the name of the alias we want to delete.
 */
void delete_alias(char *name)
{
    struct aliases *tmp = aliases;
    if (strcmp(tmp->name, name) == 0)
    {
        aliases = aliases->next;
        free(tmp->name);
        free(tmp->value);
        free(tmp);
        return;
    }

    while (tmp->next)
    {
        if (strcmp(tmp->next->name, name) == 0)
        {
            struct aliases *t = tmp->next;
            tmp->next = tmp->next->next;
            free(t->name);
            free(t->value);
            free(t);
            return;
        }
        tmp = tmp->next;
    }
}


/*!
 **  This function reproduces de behaviours of the unalias command.
 **  \param ast : the ast containing the parameter of the command.
 **  \return The exact same values as the unalias command.
 */
int eval_unalias(struct ast *ast)
{
    size_t size = nb_nodes(ast);
    if (size == 0)
    {
        printf("Use : unalias [-a] name [name ...]\n");
        return 2;
    }
    size_t i = 0;
    struct node_list *tmp = ast->child;
    if (strcmp(tmp->node->data, "-a") == 0)
    {
        free_alias();
        return 0;
    }
    int ret = 0;
    while (i < size)
    {
        if (strcmp(tmp->node->data, "-a") == 0)
        {
            printf("unalias : -a : not found\n");
            ret = 1;
        }
        else
        {
            if (find_alias(tmp->node->data))
            {
                delete_alias(tmp->node->data);
            }
            else
            {
                char *n = tmp->node->data;
                printf("unalias : %s : not found\n", n);
                ret = 1;
            }
        }
        i++;
        tmp = tmp->next;
    }
    return ret;
}
