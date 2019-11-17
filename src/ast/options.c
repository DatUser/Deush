#include "header/operations.h"
#include "header/options.h"

int is_option(char *arg)
{
    if (!my_strcmp(arg, "-c"))
    {
        c = 1;
        return 1;
    }
    if (!my_strcmp(arg, "-O") || !my_strcmp(arg, "+O"))
    {
        O = (arg[0] == '-') ? -1 : 1;
        return 1;
    }
    if (!my_strcmp(arg, "--norc"))
    {
        norc = 1;
        return 1;
    }
    if (!my_strcmp(arg, "--ast-print"))
    {
        ast_print = 1;
        return 1;
    }
    return 0;
}
