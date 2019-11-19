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

        printf("primary : %d\n", tmp->primary_type);
        printf("secondary : %d\n", tmp->secondary_type);
        printf("value : %s\n", tmp->value);

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
            free(tmp);
        }
        free(lexer);
        lexer = init_token_list();
    }
    return lexer;
}
