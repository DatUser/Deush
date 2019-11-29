/*!
**  \file lexer.h
**  \brief This is the lexer header file.
**  \author 42sh Group
*/

#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

int is_separator(char *input, size_t *index, size_t len);
int is_if(char *input, size_t *index, size_t len);
int is_then(char *input, size_t *index, size_t len);
int is_else(char *input, size_t *index, size_t len);
int is_elif(char *input, size_t *index, size_t len);
int is_fi(char *input, size_t *index, size_t len);
int is_command(char *input, size_t *index, size_t len);

int is_comment(char *input, size_t *index, size_t len);
int is_do(char *input, size_t *index, size_t len);
int is_done(char *input, size_t *index, size_t len);
int is_until(char *input, size_t *index, size_t len);
int is_while(char *input, size_t *index, size_t len);
int is_function(char *input, size_t *index, size_t len);

int is_case(char *input, size_t *index, size_t len);
int is_in(char *input, size_t *index, size_t len);
int is_WORD(char *input, size_t *index, size_t len);
int is_esac(char *input, size_t *index, size_t len);

int add_bang(char *input, size_t *index, size_t len);
int add_pipe(char *p, size_t *index);
int pipelines(char *input, size_t *index, size_t len);

int add_redirect(char *input, char *nb);
int redirection(char *input, size_t *index, size_t len);
int is_shopt(char *input, size_t *index, size_t len);

int is_for(char *input, size_t *index, size_t len);

void add_newline(void);
#endif
