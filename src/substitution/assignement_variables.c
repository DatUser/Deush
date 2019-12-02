
#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "header/assignement_variables.h"
struct variables *variables;

char *itoa(int value, char *s)
{
    int neg = 0;
    if (value < 0)
    {
        *s = '-';
        neg = 1;
        s++;
        value = -value;
    }
    int cpt = 1;
    int copy = value;
    while (copy > 10)
    {
        copy = copy / 10;
        cpt++;
    }
    *(s + cpt) = '\0';
    cpt--;
    while(cpt >= 0)
    {
        *(s + cpt) = '0' + value % 10;
        cpt--;
        value = value / 10;
    }
    if (neg == 1)
        s--;

    return s;
}

void variable_update(char *name, char *value)
{
    if (variable_value(name) != NULL)
    {
        struct variables *curr = variables;
        while(strcmp(name, curr->name) != 0)
            curr = curr->next;
        if (curr->value[0] == '$')
        {
            free(curr->value);
            char *new_value = strdup(value);
            curr->value = new_value;
        }
    }
}
void pop_variable(char *name)
{
    if (variable_value(name) != NULL)
    {
        struct variables *curr = variables;
        if (!curr->next)
        {
            free(curr->name);
            free(curr->value);
            free(curr);
            variables = NULL;
        }
        else if (strcmp(name, curr->name) == 0)
        {
            struct variables *tmp = variables;
            variables = tmp->next;
            free(tmp->value);
            free(tmp->name);
            free(tmp);
        }
        else
        {
            while(strcmp(name, curr->next->name) != 0)
                curr = curr->next;
            struct variables *tmp = curr->next;
            curr->next = curr->next->next;
            free(tmp->name);
            free(tmp->value);
            free(tmp);
        }
    }
}
void add_variable(char *name, char *value)
{
    if (variable_value(name) != NULL)
    {
        pop_variable(name);
        add_variable(name,value);
    }
    else
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
void begin_script(struct ast *ast)
{
    int cpt = 0;
    char *cpt_to_c = malloc(1024);
    if (cpt_to_c)
    {
        char *data = ast->data;
        add_variable("0", data);
        struct node_list *child = ast->child;
        while(child)
        {
            cpt++;
            cpt_to_c = itoa(cpt,cpt_to_c);
            data = child->node->data;
            add_variable(cpt_to_c,data);
            child = child->next;
        }
        add_variable("#",cpt_to_c);
        free(cpt_to_c);
    }
}

void update_random(void)
{
    int rd = generate_random();
    char *tmp = malloc(50);
    if (tmp)
    {
        tmp = itoa(rd,tmp);
        pop_variable("RANDOM");
        add_variable("RANDOM", tmp);
        free(tmp);
    }
}
void update_shellopts(void)
{
    char *tmp = shellopts();
    if (tmp)
    {
        pop_variable("SHELLOPTS");
        add_variable("$SHELLOPTS", tmp);
        free(tmp);
    }
}

void is_special(char *value)
{
    if (strcmp(value, "$RANDOM") == 0)
    {
        update_random();
    }
    else if (strcmp(value, "$SHELLOPTS") == 0)
    {
        update_shellopts();
    }
}
char *active_substitution(char *value)
{
    is_special(value);
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
        return "";
    else
    {
        if (*tmp->value == '$')
        {
            is_special(tmp->value);
            return active_substitution(tmp->value);
        }
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
        printf("declare -x %s=%s\n", tmp->name, tmp->value);
        tmp = tmp->next;
    }
}

void script_del_args(void)
{
    char *args_number = malloc(1024);
    if (args_number)
    {
        int cpt = 0;
        args_number = itoa(cpt , args_number);
        while(variable_value(args_number) != NULL)
        {
            pop_variable(args_number);
            cpt++;
            args_number = itoa(cpt , args_number);
        }
    }
    free(args_number);
    pop_variable("*");
    pop_variable("@");
    pop_variable("$");
    //pop_variable("?");
    pop_variable("#");
}
void init_variables(void)
{
    uid_t stock = getuid();
    char *numb = malloc(50);
    if (numb)
    {
        numb = itoa(stock,numb);
        add_variable("UID", numb);
        free(numb);
    }
    pid_t stock2 = getpid();
    char *numb2 = malloc(50);
    if (numb2)
    {
        numb2 = itoa(stock2,numb2);
        add_variable("$", numb2);
        free(numb2);
    }

    char *result = shellopts();
    add_variable("SHELLOPTS","1");
    free(result);
    add_variable("RANDOM", "1");
    add_variable("?","0");
}
