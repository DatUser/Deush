/*!
 **  \file lexer.c
 **  \brief This file contains all the functions related to the lexer.
 **  \author 42sh Group
 */
#define ARRAY_O_SIZE 1
#define ARRAY_WO_SIZE 1
#include "header/lexer.h"
#include "header/token.h"
#include "../auxiliary/header/auxiliary.h"
#include "../include/include.h"

struct token_list *lexer;
char * tabl_opt_reg[ARRAY_WO_SIZE]={"toto"};
int tabl_opt_reg_nbr[ARRAY_WO_SIZE]={1};

char * tabl_opt_o[ARRAY_O_SIZE]={"tata"};
int tabl_opt_o_nbr[ARRAY_O_SIZE]={1};

int is_separator(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len)
    {
        return 0;
    }
    if (*index >= len - 1 && input[*index] == '&')
    {
        struct token *to_add = init_token(T_SEPARATOR, T_ANDIF,
        cut(input, index, *index + 2, len));
        add_token(lexer, to_add);
        *index += 2;
    }
    else if (input[*index] == '|')
    {
        if (*index >= len - 1 && input[*index + 1] == '|')
        {
            struct token *to_add = init_token(T_SEPARATOR, T_ORIF,
            cut(input, index, *index + 2, len));
            add_token(lexer, to_add);
            *index += 2;
        }
        else
        {
            struct token *to_add = init_token(T_SEPARATOR, T_PIPE,
            cut(input, index, *index + 1, len));
            add_token(lexer, to_add);
            *index += 1;
        }
    }
    else if (input[*index] == '(')
    {
         struct token *to_add = init_token(T_SEPARATOR, T_PIPE,
                 cut(input, index, *index + 1, len));
         add_token(lexer, to_add);
         *index += 1;
    }
    else if (input[*index] == ')')
    {
        struct token *to_add = init_token(T_SEPARATOR, T_RPAR,
            cut(input, index, *index + 1, len));
        add_token(lexer, to_add);
        *index += 1;
    }
    else if (input[*index] == ';')
    {
        if (*index < len - 1 && input[*index + 1] == ';')
        {
            struct token *to_add = init_token(T_SEPARATOR, T_DSEMI,
            cut(input, index, *index + 2, len));
            add_token(lexer, to_add);
            *index += 2;
        }
        else
        {
            struct token *to_add = init_token(T_SEPARATOR, T_SEMI,
            cut(input, index, *index + 1, len));
            add_token(lexer, to_add);
            *index += 1;
        }
    }
    else
    {
        return 0;
    }
    return 1;
}

int is_if(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    if (tmp >= len - 1 || input[tmp] != 'i' || input[tmp + 1] != 'f'
        || input[tmp + 2] != ' ')
    {
        return 0;
    }
    tmp += 2;

    struct token *to_add = init_token(T_IF, T_WORD, cut(input, index, tmp,
    len));
    add_token(lexer, to_add);

    remove_white_space(input, &tmp, len);
    *index = tmp;

    is_command(input, index, len);

    return 1;
}

int is_then(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len - 3 || input[tmp] != 't' || input[tmp + 1] != 'h'
        || input[tmp + 2] != 'e' || input[tmp + 3] != 'n')
    {
        return 0;
    }
    tmp += 4;
    struct token *to_add = init_token(T_THEN, T_WORD,
        cut(input, index, tmp, len));
    add_token(lexer, to_add);
    to_add = NULL;

    *index = tmp;

    if (tmp == len)
    {
        return 1;
    }

    remove_white_space(input, &tmp, len);
    *index = tmp;

    is_separator(input, index, len);
    return 1;
}

int is_else(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len - 3 || input[tmp] != 'e' || input[tmp + 1] != 'l'
        || input[tmp + 2] != 's' || input[tmp + 3] != 'e')
    {
        return 0;
    }
    tmp += 4;
    struct token *to_add = init_token(T_ELSE, T_WORD,
        cut(input, index, tmp, len));
    add_token(lexer, to_add);
    to_add = NULL;

    if (tmp == len) // NEWLINE
    {
        *index = tmp;
        return 1;
    }

    remove_white_space(input, &tmp, len);
    *index = tmp;

    is_separator(input, index, len);
    return 1;
}

int is_elif(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len - 3 || input[tmp] != 'e' || input[tmp + 1] != 'l'
        || input[tmp + 2] != 'i' || input[tmp + 3] != 'f')
    {
        return 0;
    }
    tmp += 4;
    struct token *to_add = init_token(T_ELIF, T_WORD,
        cut(input, index, tmp, len));
    add_token(lexer, to_add);

    remove_white_space(input, &tmp, len);
    *index = tmp;

    is_command(input, index, len);

    return 1;
}

int is_fi(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len - 1 || input[tmp] != 'f' || input[tmp + 1] != 'i')
    {
        return 0;
    }
    struct token *to_add = init_token(T_FI, T_WORD, cut(input, &tmp, tmp + 2,
        len));
    add_token(lexer, to_add);
    *index += 2;
    if (tmp == len) // NEWLINE
        return 1;

    remove_white_space(input, index, len);;
    is_separator(input, index, len);
    return 1;
}

int is_command(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp == len)
        return 0;

    while (tmp < len && input[tmp] != ';' && input[tmp] != '\n')
    {
        tmp++;
        if (input[tmp] == ' ')
        {
            size_t tmp3 = tmp;
            if (is_separator(input, &tmp3, len))
            {
                struct token *to_add = init_token(T_COMMAND, T_NONE,
                    cut(input, index, tmp, len));
                struct token *tmp2 = lexer->head;
                if (!tmp2->next)
                {
                    to_add->next = tmp2;
                    lexer->head = to_add;
                }
                else
               {
                    while (tmp2->next->next)
                    {
                        tmp2 = tmp2->next;
                    }
                    to_add->next = tmp2->next;
                    tmp2->next = to_add;
                }
                lexer->size += 1;
                remove_white_space(input, &tmp, len);
                *index = tmp3;
                if (input[tmp] == ';')
                {
                    tmp = tmp3;
                    break;
                }
            }
        }
    }
    if (tmp != *index)
    {
        struct token *to_add = init_token(T_COMMAND, T_NONE,
            cut(input, index, tmp, len));
        add_token(lexer, to_add);
    }
    remove_white_space(input, &tmp, len);
    *index = tmp;
    is_separator(input, index, len);
    return 1;
}
int setter_opt(char *argv[],int pos[],int setter)
{
    int size ;
    int result = 0;
    if (o_activate == 0)
    {
        char *tab[] = tabl_opt_reg;
        size = ARRAY_WO_SIZE;
    }
    else
    {
        char *tab[] = tabl_opt_o;
        size = ARRAY_O_SIZE;
    }
    int position = pos[0];
    int len = pos[1];
    for (int i = position; i < len; i++)
    {
        int passed = 0;
        for (int j = 0; j < size ;j++)
        {
            if (strcmp(argv[i],tab[j]) == 0)
            {
                if (setter == 0)
                    tabl_opt_reg_nbr[j] = 0;
                else if (setter == 1)
                    tabl_opt_reg_nbr[j] = 1;
                passed = 1;
            }
        }
        if (passed == 0)
            result = 1;
    }
    return result;

}
int activate_shop(char *argv[], int argc)
{
    int u = 0;
    int q = 0;
    int s = 0;
    int o = 0;
    int cpt = 1;
    if (argc == cpt)
        return printer_tab();
    if (argv[cpt][0] == '-')
    {
        for (int i = 1; *(argv[cpt] + i) != '\0'; i++)
        {
            if (*(argv[cpt] + i) == 'u')
                u = 1;
            else if (*(argv[cpt] + i) == 'q')
                q = 1;
            else if (*(argv[cpt] + i) == 's')
                s = 1;
            else if (*(argv[cpt] + i) != 'o')
                return -1;
        }
        cpt++;
    }
    if (s == 1 && u == 1)
    {
        fprintf(stderr,"cannot set and unset shell opt");
        return -1;
    }
    int poslen[2]={cpt,argc};
    if (s == 1 && argc != 2)
        return setter_opt(argv,poslen,1); // 1 if opt not exist
    if (u == 1 && argc != 2)
        return setter_opt(argv,poslen,0);// 1 if opt not exist
    if (q == 0)
        return printer_tab();
    if ((u == 1 && argc == 2) || (s == 1 && argc == 2))
        return printer_tab();
    return 0;
}

int check_shop(char *input)
{
    char *new = strdup(input);
    char *saveptr;
    char *curr = strtok_r(new,' ', &saveptr);
    char *argv[1024];
    int i = 0;
    while(curr)
    {
        if ((strcmp(curr, " ") != 0) && (strcmp(curr,";") != 0)
            && (strcmp(curr, "\n") != 0))
        {
            char *stock = strdup(curr);
            argv[i] = stock;
            i++;
        }
        curr = strtok_r(NULL, ' ', &saveptr);
    }
    return activate_shop(argv, i);

}
int is_shopt(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len - 4 || input[*index] != 's' || input[*index + 1]  != 'h'
        || input[*index + 2] != 'o' || input[*index + 3] != 'p'
        || input[*index + 4] !='t')
    {
        return 0;
    }

    size_t tmp = *index;
    while (tmp < len && input[tmp] != ';' && input[tmp] != '\n')
    {
        tmp++;
    }
    char *string_to_shopt = cut(input, index, tmp, len);
    *index = tmp + 1;
    return 1;
}

int is_comment(char *input, size_t *index, size_t len)
{

    if (*index == len)
        return 0;
    if (input[*index] == '#')
        return 1;
    return 0;
}

int is_while(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len - 4 || input[tmp] != 'w' || input[tmp + 1]  != 'h'
        || input[tmp + 2] != 'i' || input[tmp + 3] != 'l' || input[tmp + 4] !=
        'e')
    {
        return 0;
    }
    struct token *to_add = init_token(T_WHILE, T_WORD, cut(input, &tmp, tmp + 5,
        len));
    add_token(lexer, to_add);
    *index += 5;

    is_command(input, index, len);
    return 1;
}

int is_do(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if ((*index >= len - 1 || input[*index] != 'd'
            || input[*index + 1] != 'o'))
    {
        return 0;
    }
    struct token *to_add = init_token(T_DO, T_WORD, cut(input, index,
        *index + 2, len));
    add_token(lexer, to_add);
    *index += 2;
    return 1;
}

int is_done(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len - 3 || input[*index] != 'd' || input[*index + 1] != 'o'
            || input[*index + 2] != 'n' || input[*index + 3] != 'e')
    {
        return 0;
    }
    struct token *to_add = init_token(T_DONE, T_WORD, cut(input, index,
        *index + 4, len));
    add_token(lexer, to_add);
    *index += 4;
    return 1;
}

int is_until(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len - 4 || input[*index] != 'u' || input[*index + 1] != 'n'
            || input[*index + 2] != 't' || input[*index + 3] != 'i'
            || input[*index + 4] != 'l')
    {
        return 0;
    }
    struct token *to_add = init_token(T_UNTIL, T_WORD, cut(input, index,
        *index + 5, len));
    add_token(lexer, to_add);
    *index += 5;
    return 1;
}

int is_case(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len - 3 || input[*index] != 'c' || input[*index + 1] != 'a'
        || input[*index + 2] != 's' || input[*index + 3] != 'e')
    {
        return 0;
    }
    struct token *to_add = init_token(T_CASE, T_WORD, cut(input, index, *index
        + 4, len));
    add_token(lexer, to_add);
    *index += 4;
    remove_white_space(input, index, len);
    is_WORD(input, index, len);
    return 1;
}

int is_in(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    if (*index >= len - 1 || input[*index] != 'i' || input[*index + 1] != 'n')
    {
        return 0;
    }
    struct token *to_add = init_token(T_IN, T_WORD, cut(input, index, *index
        + 2, len));
    add_token(lexer, to_add);
    *index += 2;
    return 1;
}

int is_WORD(char *input, size_t *index, size_t len)
{
    remove_white_space(input, index, len);
    size_t tmp = *index;
    if (tmp >= len || ((input[tmp] >= 48 && input[tmp] <= 57)
        || isspace(input[tmp])))
    {
        return 0;
    }
    tmp += 1;

    while (tmp < len && !(isspace(input[tmp])) && input[tmp] != ';'
            && input[tmp] != '&')
    {
        if (input[tmp] >= 1 && input[tmp] <= 6)
            break;
        if (input[tmp] >= 14 && input[tmp] <= 31)
            break;
        if (input[tmp] == 127)
            break;

        tmp++;
    }
    size_t tmp3 = tmp;
    remove_white_space(input, &tmp3, len);
    if ((tmp3 != len &&
        input[tmp3] != '&' && input[tmp3] != ';' && input[tmp] != '|' &&
        input[tmp3] != ')' && input[tmp3] != '|' && input[tmp3] != '\n')
        && input[tmp3] != 'i' && input[tmp3 + 1] != 'n'
        && input[tmp3] != 'd' && input[tmp3 + 1] != 'o')
        return 0;
    struct token *to_add = init_token(T_WORD, T_NONE, cut(input, index, tmp,
        len));
    add_token(lexer, to_add);
    *index = tmp;
    return 1;
}
/*!
**  This function adds a bang token to the token list if there is one.
**  \param input : the string that may contain a bang character.
**  \param index : the current index in the string.
**  \param len : the length of the input string.
**  \return 1 if the token could be added to the token list, -1 otherwise.
*/
int add_bang(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    remove_white_space(input, &tmp, len);
    if (input[tmp] == '!')
    {
        char *s = cut(input, index, tmp + 1, len);
        struct token *bang = init_token(T_BANG, T_NONE, s);
        if (bang == NULL)
        {
            return -1; //error
        }

        lexer = add_token(lexer, bang);
        tmp += 1;
        *index = tmp;
        return 1;
    }

    return 0;
}


/*!
**  This function adds a pipe token to the token list if there is one.
**  \param p : the string containing the pipe character onlu.
**  \param index : the actual index in the string that the input string come
**   from.
**  \return 1 if the token could be added to the token list, -1 otherwise.
*/
int add_pipe(char *p, size_t *index)
{
    size_t tmp = *index;
    struct token *pipe = init_token(T_PIPE, T_NONE, p);
    if (pipe == NULL)
    {
        return -1;
    }

    lexer = add_token(lexer, pipe);
    *index = tmp;
    return 1;
}


/*!
**  This function adds a newline token to the token list.
*/
void add_newline(void)
{
    char *s = malloc(sizeof(char) * 2);
    if (s == NULL)
    {
        return;
    }

    s = "\n";
    struct token *newline = init_token(T_NEWLINE, T_NONE, s);
    add_token(lexer, newline);
}


/*!
**  This function adds the pipeline tokens to the token list.
**  \param input : the string to be parsed.
**  \param index : the current index in the input string.
**  \param len : the length of the input string.
**  \return 1 if there is no errors, 0 otherwise.
*/
int pipelines(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    if (add_bang(input, &tmp, len) == -1)
    {
        return 0;
    }

    remove_white_space(input, &tmp, len);
    *index = tmp;

    while (tmp < len)
    {
        while (input[tmp] != '|' && tmp < len)
        {
            tmp += 1;
        }

        char *c = cut(input, index, tmp, len);
        struct token *command = init_token(T_COMMAND, T_NONE, c);
        if (command == NULL)
        {
            return 0;
        }

        lexer = add_token(lexer, command);
        *index = tmp;
        tmp += 1;

        char *p = cut(input, index, tmp, len);
        if (strcmp(p, "|") == 0)
        {
            if (add_pipe(p, &tmp) == -1)
            {
                return 0;
            }
        }

        remove_white_space(input, &tmp, len);
        *index = tmp;

        if (input[tmp] == '\n')
        {
            return 1;
        }
    }

    add_newline();
    return 1;
}



int is_esac(char *input, size_t *index, size_t len)
{
    if (*index >= len - 3 || input[*index] != 'e' || input[*index + 1] != 's'
        || input[*index + 2] != 'a' || input[*index + 3] != 'c')
    {
        return 0;
    }
    struct token *to_add = init_token(T_ESAC, T_WORD, cut(input, index, *index
    + 4, len));
    add_token(lexer, to_add);

    *index += 4;
    return 1;
}

/*!
**  This funcitons add a token operator related to the redirection in the
**  token list.
**  \param input : The string that will be the value of the token.
**  \return 1 if the token could be createad and added, 0 otherwise.
*/
int add_redirect(char *input)
{
    if (strcmp(input, ">") == 0)
    {
        struct token *t = init_token(T_GREATER, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<") == 0)
    {
        struct token *t = init_token(T_LESS, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, ">>") == 0)
    {
        struct token *t = init_token(T_RGREAT, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<<") == 0)
    {
        struct token *t = init_token(T_RLESS, T_HEREDOC, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<<-") == 0)
    {
        struct token *t = init_token(T_RLESSDASH, T_HEREDOC, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, ">&") == 0)
    {
        struct token *t = init_token(T_GREATAND, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<&") == 0)
    {
        struct token *t = init_token(T_LESSAND, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, ">|") == 0)
    {
        struct token *t = init_token(T_CLOBBER, T_NONE, input);
        add_token(lexer, t);
    }
    else if (strcmp(input, "<>") == 0)
    {
        struct token *t = init_token(T_LESSGREAT, T_NONE, input);
        add_token(lexer, t);
    }
    else
    {
        return 0;
    }

    return 1;
}

/*!
**  This function add the redirection nodes to the token list.
**  \param input : the string that contains the information.
**  \param index : the current index in the input string.
**  \param len : the length of the input string.
*/
int redirection(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    remove_white_space(input, &tmp, len);
    *index = tmp;

    while (tmp < len)
    {
        while (input[tmp] != ' ')
        {
            tmp += 1;
        }

        char *nb = cut(input, index, tmp, len);
        struct token *ionb = init_token(T_WORD, T_NONE, nb);
        if (ionb == NULL)
        {
            return 0;
        }

        add_token(lexer, ionb);
        *index = tmp;

        remove_white_space(input, &tmp, len);
        *index = tmp;

        while (input[tmp] != ' ')
        {
            tmp += 1;
        }

        char *op = cut(input, index, tmp, len);
        add_redirect(op);

        remove_white_space(input, &tmp, len);
        *index = tmp;

        while (tmp < len)
        {
            tmp += 1;
        }

        char *word = cut(input, index, tmp, len);
        struct token *w = init_token(T_WORD, T_NONE, word);
        add_token(lexer, w);
        *index = tmp;
    }

    add_newline();
    return 1;
}




int rule_for(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;

    if (strcmp(input, "") == 0)
    {
        add_newline();
        return 0;
    }

    remove_white_space(input, &tmp, len);
    *index = tmp;

    //for
    while (!isblank(input[tmp]) && tmp < len)
    {
        tmp += 1;
    }
    char *f = cut(input, index, tmp, len);
    struct token *for_token = init_token(T_FOR, T_NONE, f);
    add_token(lexer, for_token);
    *index = tmp;

    //word
    remove_white_space(input, &tmp, len);
    *index = tmp;
    while (!isblank(input[tmp]) && tmp < len)
    {
        tmp += 1;
    }
    char *w = cut(input, index, tmp, len);
    if (w != NULL)
    {
        struct token *word = init_token(T_WORD, T_NONE, w);
        add_token(lexer, word);
        *index = tmp;
    }

    //separator
    remove_white_space(input, &tmp, len);
    *index = tmp;
    if (input[tmp] == ';')
    {
        char *s = cut(input, index, tmp + 1, len);
        struct token *sepa = init_token(T_SEPARATOR, T_SEMI, s);
        add_token(lexer, sepa);
        tmp += 1;

        remove_white_space(input, &tmp, len);
        *index = tmp;

        while (input[tmp] == '\n')
        {
            add_newline();
            tmp += 1;
        }
        *index = tmp;

        is_do(input, &tmp, len);
        *index = tmp;

        is_done(input, &tmp, len);
        *index = tmp;

        return 1;

    }

    while (input[tmp] == '\n')
    {
        add_newline();
        tmp += 1;
    }

    remove_white_space(input, &tmp, len);
    *index = tmp;

    while (!isblank(input[tmp]))
    {
        tmp += 1;
    }

    char *i = cut(input, index, tmp, len);
    struct token *in_token = init_token(T_IN, T_NONE, i);
    add_token(lexer, in_token);
    *index = tmp;

    remove_white_space(input, &tmp, len);
    *index = tmp;

    while (!isblank(input[tmp]))
    {
        tmp += 1;
    }
    char *w2 = cut(input, index, tmp, len);
    struct token *word2 = init_token(T_WORD, T_NONE, w2);
    add_token(lexer, word2);

    remove_white_space(input, &tmp, len);
    *index = tmp;

    which_separator(input, &tmp, len);

    remove_white_space(input, &tmp, len);
    *index = tmp;
    while (input[tmp] == '\n')
    {
        which_separator(input, &tmp, len);
    }

    is_do(input, &tmp, len);
    *index = tmp;

    is_done(input, &tmp, len);
    *index = tmp;



    return 1;
}

/*int main(void)
{
    lexer = init_token_list();
    char *s = "for trezfdvgerf \n in tropmarant \n \n\n do ertyui done";

    //char *d = "echo 1 | echo 2";

    size_t i = 0;
    size_t len = strlen(s);

    //size_t len2 = strlen(d);
    printf("%d\n", rule_for(s, &i, len));
    //printf("%d\n", pipelines(d, &i, len2));
    token_printer(lexer);
}*/
