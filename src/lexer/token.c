/*!
**  \file token.c
**  This file contain the functions related to the token and token_list
**  data_structures.
**  \author 42sh Group
*/


/*!
**  \mainpage 42sh Documentation
**  This is the documentation of your 42sh project.\n
**  The file list is sorted by sections.\n
**  Each folder represents a section.\n
**  Those sections are :\n\n
**
**  \section Lexer
**  In this section you will find everything related to the lexer.\n\n
**
**  \section AST
**  In this section you will find everything related to the ast.\n\n
**
**  \section Prompt
**  In this section you will find everything related to the prompt.\n\n
**
**  \section Auxiliary
**  In this section you will find the auxiliary functions.\n\n
**
**  \section Include
**  In this section you will find :\n
**      - The global variables.\n
**      - Everything related to the preprocessor.\n\n
*/

#include "../include/include.h"
#include "header/token.h"


/*!
**  This function initializes a token data structure.
**  \param type1 : the primary type of the token.
**  \param type2 : the secondary type of the token.
**  \param val : the value of the token.
**  \return The token if it could be created, NULL otherwise.
*/
struct token *init_token(enum token_type type1, enum token_type type2,
    char *val)
{
    struct token *t = malloc(sizeof(struct token));
    if (t == NULL)
    {
        return NULL;
    }

    t->primary_type = type1;
    t->secondary_type = type2;
    t->value = val;
    t->next = NULL;

    return t;
}


/*!
**  This function initializes a token_list data structure.
**  \return The token_list if it could be created, NULL otherwise.
*/
struct token_list *init_token_list(void)
{
    struct token_list *t = malloc(sizeof(struct token_list));
    if (t == NULL)
    {
        return NULL;
    }

    t->size = 0;
    t->head = NULL;

    return t;
}


/*!
**  This function adds a token to a list of tokens.
**  \param list : the list of tokens.
**  \param t : the token to be appended.
**  \return The updated list of tokens.
*/
struct token_list *add_token(struct token_list *list, struct token *t)
{
    if (list->size == 0)
    {
        list->head = t;
        list->size += 1;
        return list;
    }

    struct token *tmp = list->head;
    int i = 0;
    while (tmp->next && i < list->size - 1)
    {
        tmp = tmp->next;
        i++;
    }
    tmp->next = t;
    list->size += 1;

    return list;
}

/*!
**  This function checks the type of the token and prints its equivalent.
*/
void type_printer(enum token_type type)
{
    switch (type)
    {
    case 0:
        printf("if");
        break;
    case 1:
        printf("else");
        break;
    case 2:
        printf("elif");
        break;
    case 3:
        printf("fi");
        break;
    case 4:
        printf("for");
        break;
    case 5:
        printf("while");
        break;
    case 6:
        printf("case");
        break;
    case 7:
        printf("esac");
        break;
    case 8:
        printf("until");
        break;
    case 9:
        printf("do");
        break;
    case 10:
        printf("done");
        break;
    case 11:
        printf("then");
        break;
    case 12:
        printf("&&");
        break;
    case 13:
        printf("||");
        break;
    case 14:
        printf("|");
        break;
    case 15:
        printf(";;");
        break;
    case 16:
        printf("<<");
        break;
    case 17:
        printf(">>");
        break;
    case 18:
        printf("<&");
        break;
    case 19:
        printf(">&");
        break;
    case 20:
        printf("<>");
        break;
    case 21:
        printf("<<-");
        break;
    case 22:
        printf(">|");
        break;
    case 23:
        printf("<");
        break;
    case 24:
        printf(">");
        break;
    case 25:
        printf("(");
        break;
    case 26:
        printf(")");
        break;
    case 27:
        printf("{");
        break;
    case 28:
        printf("}");
        break;
    case 29:
        printf("!");
        break;
    case 30:
        printf("in");
        break;
    case 31:
        printf("command");
        break;
    case 32:
        printf("builtin");
        break;
    case 33:
        printf("newline");
        break;
    case 34:
        printf("word");
        break;
    case 35:
        printf("none");
        break;
    case 36:
        printf("separator");
        break;
    case 37:
        printf("&");
        break;
    case 38:
        printf(";");
        break;
    case 39:
        printf("heredoc");
        break;
    case 40:
        printf("function");
        break;
    case 41:
        printf("function name");
        break;
    case 42:

        printf("varname");
        break;
    case 43:
        printf("operator");
        break;
    case 44:
        printf("equal");
        break;
    case 45:
        printf("script");
        break;
    default:
        printf("this token is not valid");
        break;
    }
}

/*!
**  This function prints the types and values of every token in the list of 
**  tokens.
**  \param list : the list of tokens.
*/
void token_printer(struct token_list *list)
{
    struct token *tmp = list->head;
    for (int i = 0; tmp && i < list->size; i++)
    {
        printf("%d\n", i);

        printf("primary_type :");
        type_printer(tmp->primary_type);
        printf("\nsecondary_type :");
        type_printer(tmp->secondary_type);
        printf("\nvalue : %s\n", tmp->value);

        tmp = tmp->next;
    }
}

/*!
**  eThis function free all the memory allocated for a token.
**  \param list : the list of tokens.
*/
void free_token(struct token *token)
{
    free(token->value);
    free(token);
}

/*!
**  Init again the lexer
*/
struct token_list *re_init_lexer(struct token_list *lexer)
{
    if (!lexer)
    {
        lexer = init_token_list();
    }
    else
    {
        while (lexer->head)
        {
            struct token *tmp = lexer->head;
            lexer->head = lexer->head->next;
            free(tmp->value);
            free(tmp);
        }
        free(lexer);
        lexer = init_token_list();
    }
    return lexer;
}
