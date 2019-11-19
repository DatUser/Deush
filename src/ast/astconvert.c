/*!
**  \brief
**  This file contains all the functions that are related to the tree build
**  and/or traversal.
**  \file astconvert.c
**  \author Xavier Gomez
**/
//#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//#include <err.h>
//#include <stdint.h>
//#include "header/operations.h"
#include "header/astconvert.h"
//#include "header/stack.h"
#include "header/stringutils.h"
//#include "exec_plus.h"
#include <stdio.h>
/*!
**  Frees the whole tree and its components
**  \param ast : Root of the tree
**/
void free_ast(struct ast *ast)
{
    if (ast)
    {
        struct node_list *tmp = ast->child;
        for (; tmp; tmp = tmp->next)
            free_ast(tmp->node);//free_ast(ast->child->node);
        //char *d = ast->data;
        //printf("|%s|\n", d);

        struct node_list *tmp2 = ast->child;
        while (tmp2)
        {
            struct node_list *next = tmp2->next;
            free(tmp2);
            tmp2 = next;
        }

        free(ast->data); //Don't know if it's obligatory yet
        free(ast);
    }
}

/*!
**  Writes the node's label in the dot file
**  \param node : Node that will have his value linked to the id
**  \param fd : File descriptor of the dot file
**  \param id : Id of the current node
**/
void write_node_label(struct ast *node, int fd, int id)
{
    char buf[4096] = { 0 };
    strcat(buf, "    ");
    char id_node [11] = { 0 };
    strcat(buf, my_itoa(id, id_node));

    strcat(buf, "   [label = \"");
    strcat(buf, node->data);
    strcat(buf, "\"]\n");

    write(fd, buf, strlen(buf));
}

/*!
**  Writes the node's link with his father in the dot file
**  \param child : Node that will have his id linked to the one of his father
**  \param fd : File descriptor of the dot file
**  \param id_node : Id of the father node
**  \param id_child : Id of the child
**/
void write_node(struct ast *child, int fd, int id_node, int id_child)
{
    if (child)
    {
        char buf[4096] = { 0 };

        strcat(buf, "    ");

        char str_node[11] = { 0 };
        strcat(buf, my_itoa(id_node, str_node));

        strcat(buf, " -> ");

        char str_child[11] = { 0 };
        strcat(buf, my_itoa(id_child, str_child));
        strcat(buf, ";\n");

        write(fd, buf, strlen(buf));
    }
}

/*!
**  Traverses the tree and writes the node's labels in the dot file
**  \param ast : Node that will have his value linked to the id
**  \param fd : File descriptor of the dot file
**  \param id : Id of the current node
**/
void write_ast_label(struct ast *ast, int fd, int *id)
{
    if (ast)
    {
        struct node_list *tmp = ast->child;
        //int id_father = *id;
        write_node_label(ast, fd, *id);

        while (tmp)
        {
            *id += 1;
            write_ast_label(tmp->node, fd, id);
            tmp = tmp->next;
        }
    }
}

/*!
**  Traverses the tree and writes the node's links with their
**  father in the dot file
**  \param node : Node that will have his value linked to the id
**  \param fd : File descriptor of the dot file
**  \param id : Id of the current node
**/
void write_ast(struct ast *ast, int fd, int *id)
{
    if (ast)
    {
        struct node_list *tmp = ast->child;
        int id_father = *id;

        while (tmp)
        {
            *id += 1;
            write_node(tmp->node, fd, id_father, *id);
            write_ast(tmp->node, fd, id);
            tmp = tmp->next;
        }
    }
}

/*!
**  Creates the dot file and calls the two traversals to write it
**  \param ast : Node that will contain the tree
**/
void create_ast_file(struct ast *ast)
{
    mode_t rights = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    int fd = open("ast.dot", (O_CREAT | O_WRONLY), rights);

    char beg[15] = "digraph G {\n";
    write(fd, beg, 12);

    int id = 0;
    write_ast_label(ast, fd, &id);
    id = 0;
    write_ast(ast, fd, &id);

    char end[3] = "}\n";
    write(fd, end, 2);

    close(fd);
}

/*!
**  Creates a node of the tree
**  \param data : Data tha the node needs to store
**  \param type : Type of the node (it is also related to the data)
**  \return On success a pointer to a new node, NULL otherwise
**/
struct ast *create_node(void *data, enum token_type type)
{
    struct ast *new = malloc(sizeof(struct ast));
    if (new)
    {
        new->type = type;
        new->data = data;
        //new->size = 0;
        new->nb_children = 0;
        new->child = NULL;
        return new;
    }
    return NULL;
}

/*!
**  Adds a child to the ast child list
**  \param ast : Node in which the child is added
**  \param child : Node that will be added in the child list
**/
void add_child(struct ast *ast, struct ast *child)
{
    struct node_list *child_item = malloc(sizeof(struct node_list));
    if (!ast->child)
    {
        ast->child = child_item;
        ast->child->node = child;
        ast->child->next = NULL;
        ast->nb_children = 1;
    }
    else
    {
        struct node_list *tmp = ast->child;
        while (tmp->next)
            tmp = tmp->next;

        tmp->next = child_item;
        tmp->next->node = child;
        tmp->next->next = NULL;
        ast->nb_children += 1;
    }
}

void /*struct token**/ test_lexer_if(void)
{
    struct token *if_lex = malloc(sizeof(struct token));
    struct token *cmd_lex = malloc(sizeof(struct token));
    struct token *then_lex = malloc(sizeof(struct token));
    struct token *fi_lex = malloc(sizeof(struct token));

    if_lex->secondary_type = T_NONE;
    if_lex->primary_type = T_IF;

    cmd_lex->secondary_type = T_NONE;
    cmd_lex->primary_type = T_NONE;

    then_lex->secondary_type = T_NONE;
    then_lex->primary_type = T_THEN;

    fi_lex->secondary_type = T_NONE;
    fi_lex->primary_type = T_FI;

    add_lexer(if_lex);
    add_lexer(then_lex);
    add_lexer(cmd_lex);
    add_lexer(fi_lex);
}

void /*struct token**/ test_lexer_command(void)
{
    struct token *cmd_lex = malloc(sizeof(struct token));

    cmd_lex->secondary_type = T_NONE;
    cmd_lex->primary_type = T_NONE;

    add_lexer(cmd_lex);
}

void add_lexer(struct token *token)
{
    if (!lexer->head)
    {
        lexer->head = token;
    }
    else
    {
        struct token *tmp = lexer->head;
        while (tmp->next)
            tmp = tmp->next;

        tmp->next = token;
    }
}

struct token *pop_lexer(void)
{
    if (!lexer->head)
        return NULL;

    struct token *tmp = lexer->head;
    lexer->head = tmp->next;

    return tmp;
}
