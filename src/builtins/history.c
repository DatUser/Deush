/*!
 **  \file history.c
 **  \brief This file contains the functions related to the history builtin.
 **  \author 42sh Group
 */


#include "../include/include.h"
#include "../lexer/header/token.h"
#include "../auxiliary/header/auxiliary.h"

int history(void)
{
    printf("history\n");
    return 1;
}

int is_history(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    if (tmp >= len - 6 || input[tmp] != 'h' || input[tmp + 1] != 'i'
            || input[tmp + 2] != 's' || input[tmp + 3] != 't'
            || input[tmp + 4] != 'o' || input[tmp + 5] != 'r'
            || input[tmp + 6] != 'y')
    {
        return 0;
    }
    tmp += 7;

    remove_white_space(input, &tmp, len);
    *index = tmp;

    if (tmp == len)
    {
        return history();
    }

    while (tmp < len && !isblank(input[tmp]))
    {
        tmp += 1;
    }
    char *s = cut(input, index, tmp, len);

    if (strcmp(s, "-c") == 0)
    {
        rl_clear_history();
        return 1;
    }
    else if (strcmp(s, "-r") == 0)
    {
        return 2;
    }
    else
    {
        return 0;
    }

    return 1;
}
