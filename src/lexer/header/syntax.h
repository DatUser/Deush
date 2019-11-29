/*!
**  \file syntax.h
**  \brief This is the syntax header file.
**  \author 42sh Group
*/

#ifndef SYNTAX_H
#define SYNTAX_H

#include "struct.h"

int is_good_grammar(void);
int do_check(struct token *actual, int *error);

#endif
