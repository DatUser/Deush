




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

#endif

