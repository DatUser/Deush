

#ifndef ASSIGNEMENT_VARIABLES_H
#define ASSIGNEMENT_VARIABLES_H

#include "../../include/global.h"
#include "../../ast/header/astconvert.h"
struct variables
{
    char *name;
    char *value;
    struct variables *next;
};

extern struct variables *variables;
void add_variable(char *name, char *value);
int is_nbr(char *name);
char *variable_value(char *name);
void free_variables(struct variables *curr);
int generate_random(void);
char *shellopts(void);
char *active_substitution(char *value);
void begin_script(struct ast *ast);
void init_variables(void);
char *delete_expansion(char *value);
void variable_update(char *name, char *value);
void print_variables(void);

void script_del_args(void);
#endif
