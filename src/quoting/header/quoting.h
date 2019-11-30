#ifndef QUOTING_H
#define QUOTING_H

void unquote(char *input);
void has_quote(char *input, size_t len);
size_t unquote_squotes(char *input, size_t *index, size_t len);
size_t unquote_dquotes(char *input, size_t *index, size_t len);

#endif

