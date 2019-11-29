#ifndef STRUCT_H
#define STRUCT_H

/*!
**  \struct token token.h "Definition"
*/
struct token
{
    enum token_type primary_type; // primary type of the token
    enum token_type secondary_type; // secondary type of the token
    char *value; // Value of the token
    struct token *next;
};

/*!
**  \struct token_list token.h "Definition"
*/
struct token_list
{
    int size; //size of the list
    struct token *head; //first token
};

#endif
