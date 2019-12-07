/*!
**  \brief
**  This file contains all the functions that are related to the parsing
**  \file parser.c
**  \author Xavier Gomez
**/
#include <stdlib.h>
#include "header/astconvert.h"
#include <stdio.h>
#include <string.h>
#include "../prompt/header/prompt.h"
/*!
**  Inits the lexer
**/
void init_lexer(void)
{
    static int first = 1;
    if (first)
    {
        first = 0;
        lexer = NULL;
    }
}

void eat_separators(void)
{
    while (lexer->head && lexer->head->primary_type == T_SEPARATOR)
    {
        struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);
    }
}

void eat_useless_separator(void)
{
    while (lexer->head && lexer->head->secondary_type == T_NEWLINE)
    {
        struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);
    }
}

void eat_excess_separator(void)
{
    while (lexer->head && lexer->head->next
        && lexer->head->next->secondary_type == T_NEWLINE)
    {
        struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);
    }
}

/*!
**  Creates the tree obtained by parsing the lexer's token
**  \return On success 0, 1 otherwise
**/
int parse(struct ast **ast)
{
    //struct ast *ast = NULL;
    //token_printer(lexer);
    if (lexer->head)
        eat_useless_separator();
    if (lexer->head)
    {
        switch (lexer->head->primary_type)
        {
        case T_COMMANDSUB:
        case T_SCRIPT:
        case T_WORD://temporary fix to avoid infinity loop
            return parse_command(ast);
        case T_COMMAND://replace by T_COMMAND whenever you merge
            return parse_command(ast);
        case T_IF:
            return parse_if(ast, 1);
        case T_WHILE:
            return parse_while(ast);
        case T_UNTIL:
            return parse_while(ast);
        case T_CASE:
            return parse_case(ast);
        case T_FUNCTION:
            return parse_function();
        case T_FUNCTION_NAME:
            return parse_function_name(ast);
        case T_FOR:
            return parse_for(ast);
        case T_BUILTIN:
            return parse_command(ast);
        case T_VARNAME:
            return parse_assignement(ast);
        default:
            break;
        }
        return 1;
    }
    return 0;
}

/*!
**  Creates a node with the type of the current lexer token  and pops it
**  \return The node created
**/
struct ast *create_node_lexer(void)
{
    struct ast *node;
    if (lexer->head->secondary_type == T_EXPAND)
        node = create_node(lexer->head->value,
            lexer->head->secondary_type);
    else
        node = create_node(lexer->head->value,
            lexer->head->primary_type);
    struct token *out = pop_lexer();
    free(out);
    return node;
}

/*!
**  Creates a node obtained by parsing a command and adds it in the tree
**  \param ast : Address of the tree
**  \return On success 0, 1 otherwise
**/

int parse_single_command(struct ast **ast)
{
    if (lexer->head)
    {
        if (lexer->head && lexer->head->primary_type != T_DSEMI)
        {
            int len = strlen(lexer->head->value);
            lexer->head->value[len - 1] = '\0';
            /*struct token *tmp = pop_lexer();
            free(tmp->value);
            free(tmp);*/
            //printf("Lexer is not NULL\n");
            struct ast *child_cmd = create_node_lexer();
            struct ast *child_separator = create_node_lexer();
            add_child(*ast, child_separator);
            add_child(child_separator, child_cmd);
            parse(&child_cmd);
        }
        return 0;
    }
    return 1;

}

int parse_command(struct ast **ast)
{
    if (lexer->head)
    {
        //eat separator
        while (lexer->head && lexer->head->secondary_type != T_NEWLINE
                && lexer->head->secondary_type != T_SEMI
                && lexer->head->secondary_type != T_AND
                && lexer->head->primary_type != T_DO
                && lexer->head->primary_type != T_DONE
                && lexer->head->primary_type != T_FI
                && lexer->head->primary_type != T_THEN
                && lexer->head->primary_type != T_ELIF
                && lexer->head->primary_type != T_ELSE
                && lexer->head->primary_type != T_IF
                && lexer->head->primary_type != T_WHILE
                && lexer->head->primary_type != T_FOR
                && lexer->head->secondary_type != T_RBRACE
                && lexer->head->primary_type != T_VARNAME)
        //lexer->head && lexer->head->primary_type == T_COMMAND)
        {
            /*struct token *tmp = pop_lexer();
            free(tmp->value);
            free(tmp);*/
            //printf("Lexer is not NULL\n");
            struct ast *child_cmd = create_node_lexer();
            parse_wordlist(&child_cmd);
            while (lexer->head->secondary_type == T_PIPE
                    || lexer->head->secondary_type == T_ORIF
                    || lexer->head->secondary_type == T_ANDIF
                    || lexer->head->primary_type == T_LESS
                    || lexer->head->primary_type == T_GREATER
                    || lexer->head->primary_type == T_CLOBBER
                    || lexer->head->primary_type == T_RGREAT
                    || lexer->head->secondary_type == T_ANDIF
                    || lexer->head->primary_type == T_OPERATOR
                    || lexer->head->primary_type == T_LESSGREAT
                    || lexer->head->primary_type == T_RLESS
                    || lexer->head->primary_type == T_GREATAND
                    || lexer->head->primary_type == T_LESSAND)
                parse_pipe(&child_cmd);

            eat_excess_separator();
            if (lexer->head && lexer->head->secondary_type != T_RBRACE
                && child_cmd->type != T_SCRIPT)
            {
                struct ast *child_separator = create_node_lexer();
                add_child(*ast, child_separator);
                add_child(child_separator, child_cmd);
            }
            else
            {
                add_child(*ast, child_cmd);
            }
        }
        parse(ast);
        return 0;
    }
    return 1;
}
int parse_word(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast,child);
        eat_useless_separator();
    }
    return 0;
}
int parse_function_name(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
    }
    return 0;
}

int parse_assignement(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        if(lexer->head->primary_type == T_OPERATOR)
        {
            struct ast *papa = create_node_lexer();
            add_child(*ast,papa);
            struct ast *child_two = create_node_lexer();
            add_child(papa,child);
            add_child(papa,child_two);
        }
    }
    return 0;
}
int parse_builtin(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        int out = 0;
        while(lexer->head->primary_type == T_WORD)
        {
            out = (out) ? out : parse_next_token(&child);
        }
        /*struct token *tmp = pop_lexer();//eat separator at the end
        free(tmp->value);
        free(tmp);*/
    }
    return 0;
}

int parse_export_alias(struct ast **ast)
{
    struct ast *name = create_node_lexer();
    struct ast *equal = create_node_lexer();
    struct ast *value = create_node_lexer();

    name->data = append_classic(equal->data, name->data);
    name->data = append_classic(value->data, name->data);

    free_ast(equal);
    free_ast(value);
    add_child(*ast, name);
    return 0;
}

/*!
**  Creates a node obtained by parsing a list of word (after in of for loop)
**  \param ast : Address of the tree
**  \return On success 0, 1 otherwise
**/
int parse_wordlist(struct ast **ast)
{
    if (lexer->head)
    {
        if ((!strcmp((*ast)->data, "alias")
            || !strcmp((*ast)->data, "export"))
            && lexer->head->primary_type == T_VARNAME)
            return parse_export_alias(ast);

        while (lexer->head && (lexer->head->primary_type == T_WORD
            || lexer->head->primary_type == T_COMMAND))
        {
            struct ast *child_cmd = create_node_lexer();
            add_child(*ast, child_cmd);
        }
        return 0;
    }
    return 1;
}

int parse_in_for(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        parse_wordlist(&child);
        //eat separator beacause it as ';' and it is useless
        struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);
    }
    return 0;
}

/*!
**  Creates a node obtained by parsing a default token and adds it in the tree
**  as child
**  \param ast : Address of the tree
**  \return On success 0, 1 otherwise
**/
int parse_next_token(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        //eat separator
        return 0;
    }
    return 1;
}

/*!
**  Creates a node obtained by parsing an then rule and adds it in the tree
**  as child
**  \param ast : Address of the tree
**  \return On success 0, 1 otherwise
**/
int parse_then(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        //eat separator
        /*struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);*/
        eat_useless_separator();
        //parse_next_token(&child);//separator
        parse(&child);
        return 0;
    }
    return 1;
}

/*!
**  Creates a node obtained by parsing an if rule and adds it in the tree
**  as child
**  \param ast : Address of the tree
**  \param is_if : specify if the function is called from if token or elif.
**  (1 -> if_token , 0 otherwise)
**  \return On success 0, 1 otherwise
**/
int parse_if(struct ast **ast, int is_if)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();//if
        add_child(*ast, child);
        int out = 0;
        eat_useless_separator();
        out = (out) ? out : parse_command/*next_token*/(&child);//condition
        //out = (out) ? out : parse_next_token(&child);//separator
        eat_useless_separator();
        out = (out) ? out : parse_then(&child);//then

        eat_useless_separator();

        if (lexer->head->primary_type == T_ELIF)
            out = (out) ?  out : parse_if(&child, 0);//elif
        if (lexer->head->primary_type == T_ELSE && is_if)
            out = (out) ? out : parse_then(&child);//else
        out = (out) ? out : parse_next_token(&child);//fi
        //eat separator
        struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);
        return 0;
    }
    return is_if;
}

int parse_in_case(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast,child);
         int out = 0;
        //eat separator
        struct token *tmp = pop_lexer();
        free(tmp->value);
        free(tmp);
        while (lexer->head->primary_type != T_ESAC)
        {
            out = (out) ? out : parse_single_command(&child);
        }
        return 0;
    }
    return 0;
}

int parse_case(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        int out = 0;
        out = (out) ? out : parse_next_token(&child);//variable before in
        out = (out) ? out : parse_in_case(&child);//the in

        out = (out) ? out : parse_next_token(&child);//the essac at the end
        return 0;
    }
    return 0;
}

int parse_do(struct ast **ast)
{
    if (lexer->head)
    {
        eat_useless_separator();

        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        int out = 0;

        eat_useless_separator();

        while (lexer->head && lexer->head->primary_type != T_DONE)
        {
            out = (out) ? out : parse(&child);//every command in the while
            eat_useless_separator();
        }
        //out = (out) ? out : parse_command(&child);//the done at the end
    }
    return 0;
}

int parse_while(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast,child);
        eat_useless_separator();
        int out = 0;
        out = (out) ? out : parse_command(&child);//command inside while
        eat_useless_separator();
        out = (out) ? out : parse_do(&child);//everythings insides
        eat_useless_separator();
        out = (out) ? out : parse_next_token(&child);//done
        return 0;
    }
    return 0;
}

int parse_for(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child = create_node_lexer();
        add_child(*ast, child);
        int out = 0;
        out = (out) ? out : parse_next_token(&child); //variable before the in
        out = (out) ? out : parse_in_for(&child);//parses the in
        out = (out) ? out : parse_do(&child);//parses the do and what is inside
        while (lexer->head && lexer->head->secondary_type == T_NEWLINE)
        {
            struct token *tmp = pop_lexer();
            free(tmp->value);
            free(tmp);
        }
        out = (out) ? out : parse_next_token(&child); //parses the done
    }
    return 0;
}

/*!
**  Parses the T_PIPE token, (as the **ast is the address of the left command
**  of the pipe this functions creates a node for pipe that will replace the
**  left command and the left command will be the 1st child of the pipe)
**/
int parse_pipe(struct ast **ast)
{
    if (lexer->head)
    {
        struct ast *child_pipe = create_node_lexer();
        struct ast *second_command = create_node_lexer();
        struct ast *first_command = *ast;

        *ast = child_pipe;
        add_child(*ast, first_command);
        add_child(*ast, second_command);

        /*int out = 0;
        out = (out) ? out : parse_command(&child);*/
    }
    return 0;
}

int parse_function(void)
{
    if (lexer->head)
    {
        struct token *tmp;
        if (lexer->head->primary_type == T_FUNCTION)
        {
            tmp = pop_lexer();//the funcdef
            free(tmp->value);
            free(tmp);
        }
        struct ast *new_tree = create_node_lexer();//name of the func
        int out = 0;
        tmp = pop_lexer();//the '('
        free(tmp->value);
        free(tmp);
        tmp = pop_lexer();//the ')'
        free(tmp->value);
        free(tmp);
        eat_useless_separator();
        tmp = pop_lexer();//the '{'
        free(tmp->value);
        free(tmp);
        eat_useless_separator();
        while(lexer->head && lexer->head->secondary_type != T_RBRACE)
        {
            out = (out) ? out : parse(&new_tree);
        }
        tmp = pop_lexer();//the '}'
        free(tmp->value);
        free(tmp);

        struct function *new = malloc(sizeof(struct function));
        if (!new)
            return 0;;
        new->next = NULL;
        new->ast = new_tree;
        new->name = new_tree->data;
        struct function *curr = function_list;
        if (curr)
        {
            while(curr->next)
                curr = curr->next;
            curr->next = new;
        }
        else
            function_list = new;
    }
    return 0;
}
