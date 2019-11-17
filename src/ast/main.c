#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/astconvert.h"
#include <criterion/criterion.h>

Test(astconvert, create_node)
{
    char tab_if[3] = "if";
    char *st_if = malloc(3);
    strcpy(st_if, tab_if);

    char tab_then[5] = "then";
    char *st_then = malloc(5);
    strcpy(st_then, tab_then);

    char tab_command[8] = "command";
    char *st_command = malloc(8);
    strcpy(st_command, tab_command);

    char tab_fi[3] = "fi";
    char *st_fi = malloc(3);
    strcpy(st_fi, tab_fi);

    struct ast *node_if = create_node(st_if, T_IF);
    struct ast *node_then = create_node(st_then, T_THEN);
    struct ast *node_command = create_node(st_command, T_NONE);//by default
    struct ast *node_fi = create_node(st_fi, T_FI);

    cr_assert_not_null(node_if);
    free_ast(node_if);

    cr_assert_not_null(node_then);
    free_ast(node_then);

    cr_assert_not_null(node_command);
    free_ast(node_command);

    cr_assert_not_null(node_fi);
    free_ast(node_fi);
}
