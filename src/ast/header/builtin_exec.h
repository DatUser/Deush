/*!
**  \file builtin_exec.c
**  \brief This is the header file for the builtins.
**  \author 42sh Group
*/


#ifndef BUILTIN_EXEC_H
#define BUILTIN_EXEC_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


struct aliases
{
    char *name;
    char *value;
    struct aliases *next;
};

struct aliases *aliases;


int eval_shopt(struct ast *ast);

size_t nb_nodes(struct ast *ast);

int is_exit(char *input);
int eval_exit(struct ast *ast);

int eval_cd(struct ast *ast);

void print_E_op(char *m, size_t len);
void print_e_op(char *m, size_t len);
int is_option(char *input);
int is_full_options(struct ast *ast);
void print_full_options(struct ast *ast);
int eval_echo(struct ast *ast);

int eval_export(struct ast *ast);
int eval_continue(struct ast *ast);
int eval_break(struct ast *ast);
int eval_source(struct ast *ast);

struct aliases *init_alias(char *name, char *value);
void add_alias(struct aliases *alias);
void print_alias(void);
void free_alias(void);
int eval_alias(struct ast *ast);


struct aliases *find_alias(char *name);
void delete_alias(char *name);
int eval_unalias(struct ast *ast);
#endif

