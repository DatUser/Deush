#ifndef OPERATIONS_H
#define OPERATIONS_H

int is_operation(char *op, int making_tokens);

int my_strcmp(char *s1, char *s2);

int exec_cmdplus(char *cmd);

char **cmd_execplus(char *func[], char *ndarg[]);

int get_type(char *op);

int error;

#endif
