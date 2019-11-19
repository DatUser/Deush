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

#endif
