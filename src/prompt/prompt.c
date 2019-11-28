    /*!
 *   \file prompt.c
 *   \brief This file contains the functions related to the prompt part.
 *   \author 42sh Group
 */

#include "../include/global.h"
#include "../include/include.h"
#include "header/prompt.h"
#include "../include/include.h"
#include "header/prompt.h"
#include "../lexer/header/lexer.h"
#include "../lexer/header/syntax.h"
//#include "../lexer/header/token.h"
#include "../ast/header/astconvert.h"
#include "../auxiliary/header/auxiliary.h"

struct histo_list *tmp_histo = NULL;

struct token_list *lexer = NULL;
struct function *function_list = NULL;

void lexe(char *input);
int get_args(FILE *in);

/*!
**  This variable is used to the ast_print option.
*/
int ast_print = 0;

/*!
**  This variable is the PS1.
*/
char *PS1 = "42sh$ ";

/*!
**  This variable is the PS2.
*/
char *PS2 = "> ";


char *home;
char *file_name = "/.42sh_history";
char *path;

static char builtins[BUILTINS_SIZE][BUILTINS_SIZE] =
{ ".", "..", "[", "alias", "bg", "bind", "break",
    "builtin", "caller", "cd", "command", "compgen",
    "complete", "compopt", "continue", "declare",
    "dirs", "disown", "echo", "enable", "eval", "exec",
    "exit", "export", "false", "fc", "fg", "getopts",
    "hash", "help", "history", "jobs", "kill", "let",
    "local", "logout", "mapfile", "popd", "printf",
    "pushd", "pwd", "read", "readarray", "readonly",
    "return", "set", "shift", "shopt", "source",
    "suspend", "test", "times", "trap", "true", "type",
    "typeset", "ulimit", "umask", "unalias", "unset",
    "wait" };

// DETERMINE IF WE ARE IN INTERACTIVE MODE
static int is_interactive(void)
{
    int tty = rl_instream ? fileno(rl_instream) : fileno(stdin);
    return isatty(tty);
}

// GIVEN IN READLINE_EXEC
static void prep_terminal(int meta_flag)
{
    if (is_interactive())
        rl_prep_terminal(meta_flag);
}

static void signal_callback_handler(int signum)
{
    signum = signum;
    // AJOUTER LES FONCTIONS DE FREE
}


/*!
 *   Read the next line
 *   \param const char *prompt : string needed for readline
 *   \return The next line read, may be NULL if EOF
 */
char *get_next_line(const char *prompt)
{
    rl_prep_term_function = prep_terminal;

    if (!is_interactive())
        prompt = NULL;

    return readline(prompt);
}


/*!
 *   Checks if the command given in argument is or not a builtin.
 *   \param char *cmd : the command.
 *   \return 1 if cmd is a builtin, 0 otherwise.
 */
int is_builtin(char *cmd)
{
    int i = 0;
    while (i < BUILTINS_SIZE)
    {
        if (strcmp(cmd, builtins[i]) == 0)
        {
            return 1;
        }
        i++;
    }
    return 0;
}


/*!
 *   Execute the command that have been separated in words
 *   \param char **s : List of words
 *   \param char *cmd : The command to execute
 *   \return exit with 2 if couldn't fork, else 0. It handle errors
 */
int execution(char **s, char *cmd)
{
    cmd = cmd;
    if (s != NULL)
    {
        int status;
        int pid = fork();

        if (pid < 0)
        {
            err(2, "Couldn't create a new process.");
        }

        if (pid == 0)
        {
            execvp(cmd, s);
            _exit(1);
        }
        else
        {
            waitpid(pid, &status, 0);
            if (status)
            {
                printf("Command '%s' not found\n", cmd);
                return WEXITSTATUS(status);
            }
            else
            {
                return 0;
            }
        }
    }
    return 0;
}


/*!
 *   Parses the line received by get_next_line
 *   \param char *line : string to parse
 *   \return char **s : array of char* (the words in line)
 */
char **parse_prompt(char *line)
{
    char *s = strdup(line);

    char **buff = malloc(sizeof(char) * 1024);

    char *tmp = strtok(s, " ");
    int i = 0;

    while (tmp)
    {
        buff[i] = tmp;
        tmp = strtok(NULL, " ");
        i++;
    }

    buff[i] = NULL;

    return buff;
}


/*!
**  This function checks the input after the "-o" option.
**  \param curr : the argument to be checked.
**  \return 1 if the "-o"'s argument is valid, 0 otherwise.
*/
int execute_o_opt(char *curr)
{
    char *options_possible[8] = { "ast_print", "dotglob", "expand_aliases",
        "extglob", "nocaseglob", "nullglob",
        "sourcepath", "xpg_echo" };
    int len = 8;
    for (int i = 0; i < len; i++)
    {
        if (strcmp(options_possible[i], curr) == 0)
            return 1;
    }
    return 0;
}


/*!
**  This function displays the ast.
**  Not done yet.
**  \return 1 always.
*/
int execute_ast_print_opt(void)
{
    ast_print = 1;
    return 1;
}


/*!
**  This function loads the resource files.
*/
void load_resource_files(void)
{
    char *path1 = "/etc/42shrc";
    char *path2 = "~/.42shrc";
    path1 = path1;
    path2 = path2;

    FILE *f1 = fopen(path1, "r");
    FILE *f2 = fopen(path2, "r");

    if (f1 != NULL)
    {
        get_args(f1);
    }

    if (f2 != NULL)
    {
        get_args(f2);
    }
}


/*!
**  This function checks all the options present in the arguments.
**  \param argv : the array storing the arguments.
**  \param argc : the number of arguments in the array argv.
*/
int check_options(char *argv[], int argc)
{
    int tmp = 1;
    int pos = 0;
    int i = 0;
    while (i < argc)
    {
        if ((strcmp(argv[i], "+o") == 0) || (strcmp(argv[i], "-o") == 0))
        {
            if ((i == argc - 1) || (execute_o_opt(argv[i + 1]) == 0))
            {
                fprintf(stderr,"-o needed a valid option after it\n");
                return 0;
            }
            i++;
        }
        else if (strcmp(argv[i], "--norc") == 0)
        {
            tmp = 0;
        }
        else if (strcmp(argv[i], "--ast-print") == 0)
        {
            execute_ast_print_opt();
        }
        else if (strcmp(argv[i],"-c") == 0)
        {
            if (i == argc - 1)
            {
                fprintf(stderr,"-c needed a string after\n");
                return 0;
            }
            else
            {
                pos = i;
                i++;
            }
        }
        else if (pos == 0)
        {
            pos = i;
        }
        i++;
    }

    if (tmp == 1)
    {
        load_resource_files();
    }

    return pos;
}

/*!
 *   Read from the input(can be a string or a file
 *   \param FILE *in: string/file Input
 *   \return 1 if everything went well , 0 otherwise
 */
int get_args(FILE *in)
{
    size_t len = 0;
    char *line = NULL;
    ssize_t read = getline(&line,&len,in);
    while (read != -1)
    {
        lexe(line);
        read = getline(&line, &len, in);
    }
    free(line);
    //token_printer(lexer);
    return 1;
}

void lexe(char *input)
{
    size_t len = strlen(input);
    size_t index = 0;
    size_t index_prev = 0;
    int return_value = 0;
    while (index < len)
    {

        //return_value += is_history(input, &index, len);

        return_value += is_for(input, &index, len);
        return_value += is_comment(input, &index, len);
        return_value += is_if(input, &index, len);
        return_value += is_then(input, &index, len);
        return_value += is_else(input, &index, len);
        return_value += is_elif(input, &index, len);
        return_value += is_fi(input, &index, len);
        return_value += is_case(input, &index, len);
        return_value += is_in(input, &index, len);
        return_value += is_esac(input, &index, len);
        return_value += is_while(input, &index, len);
        return_value += is_done(input, &index, len);
        return_value += is_do(input, &index, len);
        return_value += is_until(input, &index, len);
        return_value += is_function(input, &index, len);
        return_value += redirection(input, &index, len);
        if (return_value)
        {
            return_value = 0;
        }
        else
        {
            if (is_separator(input, &index, len))
               continue;
            if (!is_WORD(input, &index, len))
                is_command(input, &index, len);
        }
        if (index == index_prev)
        {
            char *string = calloc(sizeof(char), 2);
            string[0] = '\n';
            struct token *to_add = init_token(T_SEPARATOR, T_NEWLINE, string);
            add_token(lexer, to_add);
            break;
        }
        else
        {
            index_prev = index;
        }
    }
    /*if (index >= len)
    {
        char *string = calloc(sizeof(char), 2);
        string[0] = '\n';
        struct token *to_add = init_token(T_SEPARATOR, T_NEWLINE, string);
        add_token(lexer, to_add);
    }*/
    //token_printer(lexer);
}

void parse2(void)
{

    while (lexer->head)
    {
        char *empty_string = malloc(1);
        empty_string[0] = '\0';
        struct ast *root_node = create_node(empty_string, T_NONE);

        parse(&root_node);

        if (root_node->child)
        {
        //    create_ast_file(root_node->child->node);
            struct node_list *tmp = root_node->child;
            while (tmp)
            {
                eval_ast(/*root_node->child->node*/tmp->node);
                if (ast_print)
                    create_ast_file(/*root_node->child*/tmp->node);
                tmp = tmp->next;
            }
        }
        if (lexer->head && lexer->head->secondary_type == T_NEWLINE)
        {
            struct token *pop = pop_lexer();
            free(pop->value);
            free(pop);
        }
        free_ast(root_node);
    }

    lexer = re_init_lexer(lexer);
}

void print_hist_list(void);

/*!
**  This function launches the interactive mode.
*/
void interactive_mode(void)
{
    signal(SIGINT, signal_callback_handler);
    char *line = get_next_line(PS1);
    char *tmp;
    char *s = NULL;
    char *history_line = NULL;
    size_t to_realloc;
    size_t grammar_error = 0;
    struct token *tmp_token = NULL;
    while (line != NULL)
    {
        if (!strcmp(line, ""))
        {
            free(line);
            line = get_next_line(PS1);
            continue;
        }
        else if (line[strlen(line) - 1] == '\\')
        {
            line[strlen(line) - 1] = '\0';
            char *line2 = get_next_line(PS2);
            while (line2[strlen(line2) - 1] == '\\')
            {
                to_realloc = strlen(line) + strlen(line2) + 1;
                tmp = calloc(sizeof(char), to_realloc);
                strcpy(tmp, line);
                strncat(tmp, line2, strlen(line2) - 1);
                strcat(tmp, "\n");
                free(line);
                line = tmp;
                line2 = get_next_line(PS2);
            }
            to_realloc = strlen(line) + strlen(line2) + 2;
            tmp = calloc(sizeof(char), to_realloc);
            strcpy(tmp, line);
            strcat(tmp, line2);
            free(line);
            line = tmp;
            free(line2);
        }
        size_t i = 0;
        size_t len = strlen(line);

        //print_hist_list();

        if (is_history(line, &i, len) == 0)
        {
            line = get_next_line(PS1);
            continue;
        }
        if (strcmp(line, ""))
        {
            lexe(line);
            char *string = calloc(sizeof(char), 2);
            string[0] = '\n';
            struct token *to_add = init_token(T_SEPARATOR, T_NEWLINE, string);
            add_token(lexer, to_add);
            if (LBRA || DO || IF || LPAR)
            {
                if (!history_line)
                {
                    history_line = calloc(sizeof(char), strlen(line) + 1);
                    history_line = strcpy(history_line, line);
                }
                else
                {
                    char *temp = history_line;
                    history_line = calloc(sizeof(char), strlen(line) +
                        strlen(temp) + 3);
                    history_line = strcpy(history_line, temp);
                    history_line = strcat(history_line, " ");
                    history_line = strcat(history_line, line);
                    tmp_token = lexer->head;
                    while (tmp_token->next->next)
                    {
                        tmp_token = tmp_token->next;
                    }
                    if (tmp_token->primary_type == T_COMMAND)
                    {
                        //printf("%s\n",tmp_token->value);
                        strcat(history_line, ";");
                    }
                    free(temp);
                }
                free(line);
                //line = NULL;
                line = get_next_line(PS2);
                continue;
            }
            //token_printer(lexer);
            if (is_good_grammar())
            {
                printf("wrong grammar\n");
                add_history(line);
                s = strdup(line);
                add_line(tmp_histo, s);
                free(line);
                lexer = re_init_lexer(lexer);
                line = get_next_line(PS1);
                continue;
            }
            //token_printer(lexer);
            if (!history_line)
            {
                add_history(line);
                s = strdup(line);
                add_line(tmp_histo, s);
            }
            else
            {
                char *temp = history_line;
                history_line = calloc(sizeof(char), strlen(line) +
                        strlen(temp) + 3);
                history_line = strcpy(history_line, temp);
                history_line = strcat(history_line, " ");
                history_line = strcat(history_line, line);
                tmp_token = lexer->head;
                while (tmp_token->next->next)
                {
                    tmp_token = tmp_token->next;
                }
                if (tmp_token->primary_type == T_COMMAND)
                {
                    printf("%s\n",tmp_token->value);
                    strcat(history_line, ";");
                }
                free(temp);
                add_history(history_line);
                s = strdup(history_line);
                add_line(tmp_histo, s);
                free(history_line);
                history_line = NULL;
            }
             if ((grammar_error = is_good_grammar()))
            {
                printf("wrong grammar\n");
                lexer = re_init_lexer(lexer);
            }

            //token_printer(lexer);
            if (!grammar_error)
            {
                parse2();
            }
            //token_printer(lexer);
            //lexe_then_parse(line);
        }
        free(line);
        line = get_next_line(PS1);
    }
    free(line);
    printf("exit\n");
    //token_printer(lexer);
}

/*!
** This function is used when the input is piped into 42sh binary
*/
void redirection_mode(void)
{
    char *line = get_next_line("");
    while (line != NULL)
    {
        lexe(line);
        free(line);
        char *string = calloc(sizeof(char), 2);
        string[0] = '\n';
        struct token *to_add = init_token(T_SEPARATOR, T_NEWLINE, string);
        add_token(lexer, to_add);
        line = get_next_line("");
    }
    //token_printer(lexer);
    free(line);
    if (is_good_grammar())
    {
        printf("wrong grammar\n");
        lexer = re_init_lexer(lexer);
        return;
    }
    token_printer(lexer);
    parse2();
    //token_printer(lexer);
}


/*!
**  This function initializes the histo_list data structure.
**  \return The histo_list if it could be created, NULL otherwise.
*/
struct histo_list *init_histo_list(void)
{
    struct histo_list *new = malloc(sizeof(struct histo_list));
    if (new == NULL)
    {
        return NULL;
    }

    new->size = 0;
    new->head = NULL;
    return new;
}


/*!
**  This function initializes a line data structure.
**  \param line : The value to be stored in the line data structure.
**  \return The line data structure if it could be created, NULL otherwise.
*/
struct line *init_line(char *line)
{
    struct line *new = malloc(sizeof(struct line));
    if (new == NULL)
    {
        return NULL;
    }

    new->value = line;
    new->next = NULL;
    return new;
}


/*!
**  This function add a line into the histo_list.
**  \param list : The hiso_list data structure.
**  \param line : The line to be stored.
**  \return 1 if the line could be added to the histo_list, 0 otherwise.
*/
int add_line(struct histo_list *list, char *line)
{
    if (list->size == 0)
    {
        struct line *new = init_line(line);
        if (new == NULL)
        {
            return 0;
        }

        list->size += 1;
        list->head = new;
        return 1;
    }

    struct line *tmp = list->head;
    size_t i = 0;
    while (i < list->size - 1)
    {
        tmp = tmp->next;
        i++;
    }

    struct line *new = init_line(line);
    if (new == NULL)
    {
        return 0;
    }

    list->size += 1;
    tmp->next = new;
    return 1;
}

/*!
**  This function clears the histo_list data structure.
*/
struct histo_list *clear_histo_list(struct histo_list *list)
{
    list->size = 0;
    return list;
}


/*!
**  This function destroys a line data structure.
**  \param l : The line to be destroyed.
*/
void destroy_hist(struct line *l)
{
    struct line *tmp = l;
    size_t i = 0;
    while (i < tmp_histo->size)
    {
        struct line *t = tmp;
        if (!tmp->next)
        {
            free(t->value);
            free(t);
            return;
            //tmp = tmp->next;
        }
        tmp = tmp->next;
        free(t->value);
        free(t);
        i++;
    }
    l = NULL;
}


/*!
**  This function displays the history of the 42sh.
**  \return 1 if the history could be displayed, 0 otherwise.
*/
int history(void)
{
    FILE *f = fopen(home, "r");
    if (f == NULL)
    {
        return 0;
    }
    char *l = NULL;
    size_t len = 0;
    ssize_t r = 0;

    while ((r = getline(&l, &len, f)) != -1)
    {
        printf("%s", l);
    }
    free(l);
    fclose(f);
    struct line *tmp = tmp_histo->head;
    size_t i = 0;
    while (i < tmp_histo->size)
    {
        printf("%s\n", tmp->value);
        tmp = tmp->next;
        i++;
    }
    return 0;
}


/*!
**  This function checks if the input is a 'history' command.
**  \param input : The string to be checked.
**  \param index : the actual index in the input string.
**  \param len : the length of the input string.
**  \return 0 if the input is a 'history' command, 1 otherwise.
*/
int is_history(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    if (tmp >= len - 6 || input[tmp] != 'h' || input[tmp + 1] != 'i'
            || input[tmp + 2] != 's' || input[tmp + 3] != 't'
            || input[tmp + 4] != 'o' || input[tmp + 5] != 'r'
            || input[tmp + 6] != 'y')
    {
        return 1;
    }
    tmp += 7;

    remove_white_space(input, &tmp, len);
    *index = tmp;

    if (tmp == len)
    {
        free(input);
        history();
        return 0;
    }

    while (tmp < len && !isblank(input[tmp]))
    {
        tmp += 1;
    }
    char *s = cut(input, index, tmp, len);

    if (strcmp(s, "-c") == 0)
    {
        free(input);
        free(s);
        rl_clear_history();
        destroy_hist(tmp_histo->head);
        tmp_histo->size = 0;
        //clear_histo_list(tmp_histo);
        return 0;
    }
    else if (strcmp(s, "-r") == 0)
    {
        free(input);
        free(s);
        FILE *f = fopen(path, "r");
        if (f == NULL)
        {
            return 0;
        }
        char *l = NULL;
        size_t len = 0;
        ssize_t r = 0;

        while ((r = getline(&l, &len, f)) != -1)
        {
            char *s = strdup(l);
            if (strcmp(s, "\n"))
            {
                free(s);
            }
            else
            {
                add_line(tmp_histo, s);
            }
            add_history(l);
        }
        free(l);
        fclose(f);

        return 0;
    }
    else
    {
        return 1;
    }


    return 0;
}


/*!
**  This function frees the token_list.
*/
void free_token_list()
{
    if (lexer)
    {
        struct token *tmp = lexer->head;
        while (tmp != NULL)
        {
            struct token *t = tmp;
            tmp = tmp->next;
            free(t->value);
            free(t);
        }
    }
    free(lexer);
}


/*!
**  This function frees the HIST_ENTRY list.
**  \param list : the list to be destroyed.
**  \param size : the size of the list.
*/
void free_hist_entry(HIST_ENTRY **list, int size)
{
    for (int i = 0; i < size; i++)
    {
        HIST_ENTRY *tmp = list[i];
        free(tmp->line);
        free(tmp->timestamp);
        free(tmp->data);
        free(tmp);
    }
    free(list);
}


/*!
**  This function displays the data stored in the hist_list
*/
void print_hist_list()
{
    struct line *tmp = tmp_histo->head;
    while (tmp)
    {
        printf("%s\n", tmp->value);
        tmp = tmp->next;
    }
}


int main(int argc, char *argv[])
{
    using_history();
    tmp_histo = init_histo_list();
    home = getenv("HOME");
    path = strcat(home, file_name);

    lexer = init_token_list();
    if (argc == 1 && is_interactive())
    {
        load_resource_files();
        execute_ast_print_opt();
        interactive_mode();
    }
    else if (argc == 1)
    {
        load_resource_files();
        redirection_mode();
    }
    else
    {
        int i = 1;
        FILE *in;
        int pos = check_options(argv, argc);
        if (pos <= 0)
            return 0;

        if (strcmp(argv[pos], "-c") == 0)
        {
            lexe(argv[pos + 1]);
            if (is_good_grammar())
            {
                printf("wrong grammar\n");
                lexer = re_init_lexer(lexer);
                return 1;
            }
            parse2();
            //token_printer(lexer);
            //lexe_then_parse(argv[pos + 1]);
            //add a teminated /n if u want to have the same output that a file
        }
        else
            in = fopen(argv[pos],"r");

        if (!in)
            return 126;

        get_args(in);
        if (is_good_grammar())
        {
            printf("wrong grammar\n");
            lexer = re_init_lexer(lexer);
            fclose(in);
            return 1;
        }
        parse2();
        fclose(in);
        i++;
    }

    HISTORY_STATE *hist = history_get_history_state ();
    HIST_ENTRY **list = history_list ();

    FILE *f = fopen(path, "a+");
    if (f == NULL)
    {
        return 1;
    }

    for (int i = 0; i < hist->length; i++)
    {
        //if (!strcmp(list[i]->line, "\n"))
        //{
            fprintf(f, "%s", list[i]->line);
            fprintf(f, "\n");
        //}
    }
    if (tmp_histo)
    {
        destroy_hist(tmp_histo->head);
    }
    free(tmp_histo);

    fclose(f);
    free_hist_entry(list, hist->length);
    free(hist);
    lexer = re_init_lexer(lexer);
    free(lexer);
    return 0;
 }
