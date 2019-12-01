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
    struct ast *ast;
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

struct histo_list
{
    size_t size;
    struct line *head;
};

struct line
{
    char *value;
    struct line *next;
};


int get_args(FILE *in);

struct histo_list *init_histo_list(void);
struct line *init_line(char *line);
int add_line(struct histo_list *list, char *line);
struct histo_list *clear_histo_list(struct histo_list *list);
int history(void);
int is_history(char *input, size_t *index, size_t len);

void lexe(char *input);
void parse2(struct ast *ast);

void redirection_mode(int save_fd, int file_fd);

#endif
