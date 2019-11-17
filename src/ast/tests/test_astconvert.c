#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/astconvert.h"
#include <criterion/criterion.h>
//#include "header/astconvert.h"
//#include "header/options.h"

/*void init_options(void)
{
    static int first_time = 1;

    if (first_time)
    {
        c = 0;
        O = 0;
        norc = 0;
        ast_print = 0;
    }
}*/

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

    //if (node_if)
    cr_assert_not_null(node_if);
    free_ast(node_if);

    cr_assert_not_null(node_then);
    free_ast(node_then);

    cr_assert_not_null(node_command);
    free_ast(node_command);

    cr_assert_not_null(node_fi);
    free_ast(node_fi);
}

Test(astconvert, add_child)
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

    //ADD CHILDREN
    add_child(node_if, node_then);
    add_child(node_if, node_command);
    add_child(node_if, node_fi);

    //if (node_if)
    cr_assert_not_null(node_if);
    //free_ast(node_if);

    cr_assert_not_null(node_if->child->node);
    //free_ast(node_then);

    cr_assert_not_null(node_if->child->next->node);
    //free_ast(node_command);

    cr_assert_not_null(node_if->child->next->next->node);
    free_ast(node_if);
}

Test(astconvert, parse_command)
{
    init_lexer();
    test_lexer_command();

    struct ast *ast = NULL;
    int error = parse_command(&ast);

    cr_assert_not_null(ast);
    cr_assert_eq(0, error);
    cr_assert_null(lexer);

    free_ast(ast);

    ast = NULL;
    error = parse_command(&ast);

    cr_assert_null(ast);
    cr_assert_eq(1, error);

    free_ast(ast);
}

/*int main(void)
{
    if (test_create_node())
        printf("create_node and free_ast working\n");

    return 0;
}*/
