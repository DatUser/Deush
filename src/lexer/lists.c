/*!
 **  \file if.c
 **  \brief This file contains all the functions related to the 'if' lexing.
 **  \author 42sh Group
 */

#include "header/lexer.h"
#include "header/token.h"
#include "../include/include.h"
#include "../auxiliary/header/auxiliary.h"

int is_compound_list_break(char *input, size_t *index, size_t len)
{
    size_t tmp = *index;
    while (tmp < len && input[index] == '\n')
    {
        tmp++;
    }


