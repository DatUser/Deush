
#include "../include/global.h"
#include "header/astconvert.h"
#include "header/stringutils.h"
#include "../prompt/header/prompt.h"
#include "header/builtin_exec.h"

int printer_shopt(int setted)
{
    for (int i = 0; i < 8; i++)
    {
        if (setted == 0)
        {
            if (shopt_opt_nbr[i] == 0)
                printf("%s  off\n", shopt_opt[i]);
            else
                printf("%s  on\n", shopt_opt[i]);
        }
        if (setted == 1)
        {
            if (shopt_opt_nbr[i] == 0)
                printf("%s  off\n", shopt_opt[i]);
        }
        if (setted == 2)
        {
            if (shopt_opt_nbr[i] == 1)
                printf("%s  on\n", shopt_opt[i]);
        }
    }
    return 0;
}
int un_set_shopt(struct node_list *curr, int setter)
{
    int result = 0;
    while (curr)
    {
        int passed = 0;
        for (int i = 0; i < 8; i++)
        {
            if (strcmp((char*) curr->node->data, shopt_opt[i]) == 0)
            {
                shopt_opt_nbr[i] = setter;
                passed = 1;
            }
        }
        if (passed == 0)
        {
            fprintf(stderr, "%s : invalid shell opt namei\n",
                    (char*)curr->node->data);
            result = 1;
        }
        curr = curr->next;
    }
    return result;
}
int checker_shopt(struct node_list *curr)
{
    int passed = 0;
    while (curr)
    {
        for (int i = 0; i < 8; i++)
        {
            if (strcmp((char*) curr->node->data, shopt_opt[i]) == 0)
            {
                if (shopt_opt_nbr[i] == 0)
                passed = 1;
            }
        }

    }
    return passed;
}

int eval_shopt(struct ast *ast)
{
    if (!ast->child)
    {
        printer_shopt(0);
        return 0;
    }
    int s = 0;
    int q = 0;
    int u = 0;
    struct node_list *curr = ast->child;
    char *tmp = curr->node->data;
    while(curr && *(tmp) == '-')
    {
        tmp = curr->node->data;
        for (int i = 1; *(tmp + i) != '\0'; i++)
        {
            if (*(tmp+ i) == 's')
                s = 1;
            else if (*(tmp + i) == 'q')
                q = 1;
            else if (*(tmp + i) == 'u')
                u = 1;
            else
            {
                fprintf(stderr,"-%c is not a valid opt", *(tmp + i));
                return 0;
            }
        }
        curr = curr->next;
    }
    if (s == 1 && u == 1)
    {
        fprintf(stderr,"can't set and unset an shopt opt\n");
        return 0;
    }
    if (s == 1 && q == 0 && curr->next == NULL)
        return printer_shopt(2);
    if (u == 1 && q == 0 && curr->next == NULL)
        return printer_shopt(1);
    if (q == 1 && curr->next == NULL)
        return 0;
    if (s == 1 && q == 0)
        return un_set_shopt(curr, 1);
    if (u == 1 && q == 0)
        return un_set_shopt(curr, 0);
    if (q == 1)
        return checker_shopt(curr);
    return 0;


}
