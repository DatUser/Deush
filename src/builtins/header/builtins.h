#include <readline/readline.h>
#include <readline/history.h>

int history(void);
int is_history(char *input, size_t *index, size_t len);
int print_history(void);
void create_history(void);
