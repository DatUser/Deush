/*!
**  \file cut.c
**  \brief This file contains the cut function.
**  \author 42sh Group
*/

#include "../include/include.h"


/*!
**  This function cuts a part of an imput string without affecting the input.
**  \param string : the string to be cut.
**  \param index : the current position in the input string.
**  \param tmp : the ending position.
**  \param len : the length of the string.
**  \return the smaller string cut from the input, NULL otherwise.
*/
char *cut(char *string, size_t *index, size_t tmp, size_t len)
{
    size_t size = tmp - *index;
    if (size == 0)
    {
        return NULL;
    }
    if (size + *index > len)
    {
        size = len - tmp;
    }

    char *res = calloc(sizeof(char), size + 1);
    if (res == NULL)
    {
        errx(2, "malloc failed");
    }

    if (size != 1 && string[*index] == '\n')
    {
        *index += 1;
    }

    for (size_t i = *index; i < tmp; i++)
    {
        res[i - *index] = string[i];
    }
    return res;
}

void remove_white_space(char *input, size_t *index, size_t len)
{
    while (*index < len && input[*index] != '\0' && isblank(input[*index]))
    {
        *index += 1;
    }
}
