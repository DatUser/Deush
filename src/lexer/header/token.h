    /*!
**  \file token.h
**  \brief This header file contains all the methods related to the lexer.
**  \author 42sh Group
*/

/*!
**  \enum token_type List of the tokens.
*/
enum token_type
{
/*  RESERVED WORDS  */
    T_IF = 0, // 'if' 0
    T_ELSE, // 'else' 1
    T_ELIF, // 'elif' 2
    T_FI, // 'fi' 3
    T_FOR, // 'for' 4
    T_WHILE, // 'while' 5
    T_CASE, // 'case' 6
    T_ESAC, // 'esac' 7
    T_UNTIL, // 'until 8
    T_DO, // 'do' 9
    T_DONE, // 'done' 10
    T_THEN, // 'then' 11

/*  OPERATORS   */
    T_ANDIF, // '&&' 12
    T_ORIF, // '||' 13
    T_PIPE, // '|' 14
    T_DSEMI, // ';;' 15

    T_RLESS, // '<<' 16
    T_RGREAT, // '>>' 17
    T_LESSAND, // '<&' 18
    T_GREATAND, // '>&' 19
    T_LESSGREAT, // '<>' 20
    T_RLESSDASH, // '<<-' 21
    T_CLOBBER, // '>|' 22

/*                     */
    T_LESS, // '<' 23
    T_GREATER, // '>' 24
    T_LPAR, // '(' 25
    T_RPAR, // ')' 26
/*                     */

/*  SPECIAL RESERVED WORDS (recognized only when reserved words are)  */
    T_LBRACE, // '{' 27
    T_RBRACE, // '}' 28
    T_BANG, // '!' 29
    T_IN, // 'in' 30

    T_COMMAND, // 31
    T_BUILTIN, // 32
    T_NEWLINE, // 33
    T_WORD, // 34
    T_NONE, // NONE


    T_SEPARATOR, //
    T_AND, // '&'
    T_SEMI, // ';'
    T_HEREDOC, // pour les redirections
    T_FUNCTION,
    T_FUNCTION_NAME,
    T_VARNAME,
    T_OPERATOR,
    T_EQUAL,
    T_SCRIPT,
    T_EXPAND,
    T_COMMANDSUB,

    T_NUMBER, // Number
    T_NNUMBER, // Negative number
    T_POWER, // **
    T_MULT, // *
    T_DIV, // /
    T_REM, // %
    T_PLUS, // +
    T_MINUS, // -
    T_MAND, // &
    T_MOR, // |
    T_XOR, // ^
    T_BITWISE, // ~
    T_DEQUAL, // ==
    T_LNEQUAL, // !=
    T_LEQUAL, // <=
    T_SEQUAL, // >=
    T_ARITHMETIC,
};

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


extern struct token_list *lexer;

struct token *init_token(enum token_type type1, enum token_type type2,
    char *val);
struct token_list *init_token_list(void);
struct token_list *add_token(struct token_list *list, struct token *t);
void token_printer(struct token_list *list);
struct token_list *re_init_lexer(struct token_list *lexer);

int is_good_grammar(void);
struct token *do_check(struct token *actual, int *error);
struct token *command_check(struct token *actual, int *error);
