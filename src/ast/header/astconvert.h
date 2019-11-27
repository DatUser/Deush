/*!
**  \brief
**  This file contains the tree structure and functions from other files
**  needed to build the tree.
**  \file astconvert.h
**  \author Xavier Gomez
**/
#ifndef ASTCONVERT_H
#define ASTCONVERT_H

#include <stddef.h>
#include "../../lexer/header/token.h"

struct node_list
{
    struct ast *node;
    struct node_list *next;
};


/*!
**  Stores the data in a tree
**/
struct ast
{
    enum token_type type;
    void *data;
    //size_t size;
    size_t nb_children;
    struct node_list *child;
};

//struct lexer *lexer;

//ASTCONVERT_C
struct ast *create_node(void *data, enum token_type type);
void create_ast_file(struct ast *ast);
//struct ast *build_tree(struct token *token);
void add_child(struct ast *ast, struct ast *child);
void free_ast(struct ast *ast);
void /*struct token **/test_lexer_command(void);
void add_lexer(struct token *token);
struct token *pop_lexer(void);

//PARSER_C
void init_lexer(void);
int parse(struct ast **ast);
int parse_command(struct ast **ast);
int parse_if(struct ast **ast, int is_if);
int parse_while(struct ast **ast);
int parse_do(struct ast **ast);
int parse_for(struct ast **ast);
int parse_pipe(struct ast **ast);
int parse_function_name(struct ast **ast);
int parse_next_token(struct ast **ast);
int builtin_choose(struct ast **ast);
int parse_case(struct ast **ast);
int parse_function(void);

int parse_pipe(struct ast **ast);

//STRINGUTILS_C
char **cut_line(struct ast *ast, size_t *len);
char *append(char *src, char *dest);
int is_num(char *s);
//AST_EVALUATION_C
int eval_ast(struct ast *ast);
int eval_pipe(struct ast *ast);
int eval_or(struct ast *ast);
int eval_and(struct ast *ast);

//REDIRECTION_C
int eval_redirect_left(struct ast *ast, int targetfd);
int eval_redirect_right(struct ast *ast, int sourcefd);
int eval_redirect_both(struct ast *ast, int sourcefd);
int eval_redirect_double_left(struct ast *ast, int targetfd);
int eval_redirect_right_and(struct ast *ast);
int eval_redirect_left_and(struct ast *ast);

#endif
