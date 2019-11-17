/*!
**  \file lexer.h
**  \brief This file contains all the methods related to the lexer.
**  \author 42sh Group
*/

#define RW_SIZE 16

static char reserved_words[RW_SIZE][RW_SIZE] = { "!", "do", "esac", "in", "{",
    "}", "done", "fi", "then", "elif", "for", "until", "case", "else", "if",
    "while" };

void remove_white_space(char *input, size_t *index, size_t len);

int is_io_file(char *input, size_t *index, size_t len);
int is_io_here(char *input, size_t *index, size_t len);
int is_IO_NUMBER(char *input, size_t *index, size_t len);
int is_io_redirect(char *input, size_t *index, size_t len);

int has_equal_character(char *input, size_t *index);
int is_ASSIGNMENT_WORD(char *input, size_t index, size_t len);

int is_cmd_prefix(char *input, size_t *index, size_t len);
int is_cmd_word(char *input, size_t *index, size_t len);
int is_cmd_suffix(char *input, size_t *index, size_t len);
int is_cmd_name(char *input, size_t *index, size_t len);

int is_simple_command(char *input, size_t *index, size_t len);
int is_command(char *input, size_t *index, size_t len);

int is_compound_command(char *input, size_t *index, size_t len);
int is_compound_list(char *input, size_t *index, size_t len);

int is_separator(char *input, size_t *index, size_t len);

int is_redirect_list(char *input, size_t *index, size_t len);

int is_fname(char *input, size_t *index, size_t len);
int is_function_body(char *input, size_t *index, size_t len);
int is_function_definition(char *input, size_t *index, size_t len);


int is_pipe_sequence(char *input, size_t *index, size_t len);
int is_pipeline(char *input, size_t *index, size_t len);

int is_and_or(char *input, size_t *index, size_t len);
int is_if_clause(char *input, size_t *index, size_t len);

int is_term(char *input, size_t *index, size_t len);

int is_elif_then(char *input, size_t *index, size_t len);
int is_else_part(char *input, size_t *index, size_t len);

int is_WORD(char *input, size_t *index, size_t len);

int is_separator_op(char *input, size_t *index, size_t len);
int is_separator(char *input, size_t *index, size_t len);

int is_newline_list(char *input, size_t *index, size_t len);
int is_linebreak(char *input, size_t *index, size_t len);

size_t my_strlen(char *input, size_t *index);
