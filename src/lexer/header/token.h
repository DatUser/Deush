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
    T_IF = 0, // 'if'
    T_ELSE, // 'else'
    T_ELIF, // 'elif'
    T_FI, // 'fi'
    T_FOR, // 'for'
    T_WHILE, // 'while'
    T_CASE, // 'case'
    T_ESAC, // 'esac'
    T_UNTIL, // 'until'
    T_DO, // 'do'
    T_DONE, // 'done'
    T_THEN, // 'then'

/*  OPERATORS   */
    T_ANDIF, // '&&'
    T_ORIF, // '||'
    T_PIPE, // '|'

    T_SEMI, // ';;'
    T_RLESS, // '<<'
    T_RGREAT, // '>>'
    T_LESSAND, // '<&'
    T_GREATAND, // '>&'
    T_LESSGREAT, // '<>'
    T_RLESSDASH, // '<<-'
    T_CLOBBER, // '>|'

/*                     */
    T_LESS, // '<'
    T_GREATER, // '>'
    T_LPAR, // '('
    T_RPAR, // ')'
/*                     */

/*  SPECIAL RESERVED WORDS (recognized only when reserved words are)  */
    T_LBRACE, // '{'
    T_RBRACE, // '}'
    T_BANG, // '!'
    T_IN, // 'in'

    T_COMMAND,
    T_NEWLINE,
    T_WORD,
    T_NONE, // NONE
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
