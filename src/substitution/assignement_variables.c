
#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "header/assignement_variables.h"
struct variables *variables;

void variable_update(char *name, char *value)
{
    if (variable_value(name) != NULL)
    {
        struct variables *curr = variables;
        while(strcmp(name, curr->name) != 0)
            curr = curr->next;
        free(curr->value);
        char *new_value = strdup(value);
        curr->value = new_value;
    }
    else
        add_variable(name, value);
}
void add_variable(char *name, char *value)
{
    struct variables *new = malloc(sizeof(struct variables));
    if (new)
    {
        char *new_name = strdup(name);
        char *new_value = strdup(value);
        new->name = new_name;
        new->value = new_value;
        new->next = NULL;
        struct variables *curr = variables;
        if (!curr)
            variables = new;
        else
        {
            while (curr->next)
                curr = curr->next;
            curr->next = new;
        }
    }
}

int is_nbr(char *name)
{
    struct variables *curr = variables;
    while (curr && strcmp(curr->name, name) != 0)
        curr = curr->next;
    if (!curr)
        return 1;
    for (int i = 0; *(curr->value + i) != '\0'; i++)
    {
        if (*(curr->value + i) < '0' || *(curr->value + i) > '9')
            return 1;
    }
    return 0;
}

char *variable_value(char *name)
{
    struct variables *curr = variables;
    while (curr && strcmp(curr->name, name) != 0)
        curr = curr->next;
    if (!curr)
        return NULL;
    else
        return curr->value;
}

void free_variables(struct variables *curr)
{
    if (curr)
    {
        free_variables(curr->next);
        free(curr->name);
        free(curr->value);
        free(curr);
    }
}


int generate_random(void)
{
    return rand() % 32767;
}

char *shellopts(void)
{
    int cpt = 0;
    for (int i = 0; i < 8; i++)
    {
        if (shopt_opt_nbr[i] == 1)
            cpt = cpt + strlen(shopt_opt[i]) + 1;
    }
    char *result = malloc(sizeof(char) * cpt);
    cpt = 0;
    for (int i = 0; i < 8; i++)
    {
        if (shopt_opt_nbr[i]== 1)
        {
            for (size_t j = 0; j < strlen(shopt_opt[i]); j++)
            {
                *(result + cpt) = *(shopt_opt[i] + j);
                cpt++;
            }
            *(result + cpt) = ':';
        }
    }
    *(result + cpt) = '\0';
    return result;
}

char *IFS(void)
{
    char *result = " \t\n";
    return result;
}

int simple_dol(char *name)
{
    if (*(name + 1) == '{')
        return 1;
    else
        return 0;
}
char *active_substitution(char *value)
{
     if (simple_dol(value) == 0)
    {
        value = (value + 1);
    }
    else
    {
        value = (value + 2);
        *(value + strlen(value) - 1) = '\0';
    }
    struct variables *tmp = variables;
    while (tmp && strcmp(value, tmp->name) != 0)
        tmp = tmp->next;
    if (!tmp)
        return " ";
    else
    {
        if (*tmp->value == '$')
            return active_substitution(tmp->value);
        else
            return tmp->value;
    }
}
void check_substitution(void)
{
    struct variables *curr = variables;
    while(curr)
    {
        if (*curr->value == '$')
            curr->value = active_substitution(curr->value);
        curr = curr->next;
    }
}

char *delete_expansion(char *value)
{
    if (simple_dol(value) == 0)
    {
        value = (value + 1);
    }
    else
    {
        value = (value + 2);
        *(value + strlen(value) - 1) = '\0';
    }
    return value;
}
void print_variables(void)
{
    struct variables *tmp = variables;
    while (tmp)
    {
        printf("%s = %s\n", tmp->name, tmp->value);
        tmp = tmp->next;
    }
}
