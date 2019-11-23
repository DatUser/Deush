/*!
**  \file prompt.h
**  \brief This is the prompt header file.
**  \author 42sh Group
*/

#ifndef SRC_H
#define SRC_H

// BASIC INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <sys/wait.h>

struct function {
    char *name;
    struct ast **ast;
    struct function *next;
};

extern struct function *function_list;

// ---------------------------------------PROMPT_C
#include <readline/readline.h>
#include <readline/history.h>

#define BUILTINS_SIZE 61

// GET THE NEXT LINE
char *get_next_line(const char *prompt);

int execution(char **s, char *cmd);
int is_builtin(char *cmd);

// ---------------------------------------END PROMPT_C



#endif
