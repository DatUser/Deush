

#ifndef ASSIGNEMENT_VARIABLES_H
#define ASSIGNEMENT_VARIABLES_H

#include "../../include/global.h"
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
char *delete_expansion(char *value);

void variable_update(char *name, char *value);
void print_variables(void);

#endif
