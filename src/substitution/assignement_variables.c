

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "assignement_variables.h"
struct variables *variables = NULL;
void add_variable(char *name, char *value)
{
    struct variables *new = malloc(sizeof(struct variables));
    if (new)
    {
        new->name = name;
        new->value = value;
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
        if (shopt_opt_nbr[i] == 1)
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


char *active_substitution(struct variables *curr)
{
    char *testing = (curr->value + 1);
    struct variables *tmp = curr->next;
    while (tmp && strcmp(testing, tmp->name) != 0)
        tmp = tmp->next;
    if (!tmp)
        return " ";
    else
    {
        if (*tmp->value == '$')
            return active_substitution(tmp);
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
            curr->value = active_substitution(curr);
        curr = curr->next;
    }
}

