#include "../include/include.h"
#include "../include/global.h"
#include "header/quoting.h"
#include "../lexer/header/token.h"
#include "../auxiliary/header/auxiliary.h"

int SQUO = 0;
int DQUO = 0;

void has_quote(char *input, size_t len)
{
    size_t index = 0;
    while (index < len)
    {
        if (input[index] == '\\')
        {
            index++;
        }
        else if (input[index] == '\'' && !DQUO)
        {
            SQUO = !SQUO;
            do
            {
                index++;
            }
            while (SQUO && index < len && input[index] != '\'');
            if (index != len && input[index] == '\'')
            {
                SQUO = 0;
            }
        }
        else if (input[index] == '\"' && !SQUO)
        {
            DQUO = !DQUO;
            do
            {
                index++;
            }
            while (DQUO && index < len && input[index] != '\"');
            if (index != len && input[index] == '\"')
            {
                DQUO = 0;
            }
        }
        index++;
    }
}

void shift(char *input, size_t index, size_t len)
{
    while (index < len - 1)
    {
        input[index] = input[index + 1];
        index++;
    }
    input[len - 1] = '\0';
}

size_t unquote_backslashes(char *input, size_t *index, size_t len)
{
    while (*index < strlen(input))
    {
        if (input[*index] == '\\')
        {
            shift(input, *index, strlen(input));
            len--;
        }
        else if (input[*index] == '\'')
        {
            unquote_squotes(input, index, strlen(input));
        }
        *index += 1;

    }
    return len;
}

size_t unquote_squotes(char *input, size_t *index, size_t len)
{
    shift(input, *index, len);
    //*index += 1;
    SQUO = !SQUO;
    if (!SQUO)
    {
        return len - 1;
    }
    while (*index < len && input[*index] != '\'')
    {
        if (input[*index] == '\n')
        {
            input[*index] = '\r';
        }
        *index += 1;
    }
    if (*index != len)
    {
        shift(input, *index, len);
        SQUO = 0;
        return len - 1;
    }
    return len;
}

size_t unquote_dquotes(char *input, size_t *index, size_t len)
{
    shift(input, *index, len);
    DQUO = !DQUO;
    if (!DQUO)
    {
        return len - 1;
    }
    size_t tmp = *index;
    while (*index < len && input[*index] != '\"')
    {
        if (input[*index] == '\\' && *index < len - 1)
        {
            if (input[*index + 1] == '$' || input[*index + 1] == '\\'
                || input[*index + 1] == '`' || input[*index + 1] == '\n'
                || input[*index + 1] == '\"')
            {
                shift(input, *index, len);
            }
        }
        if (input[*index] == '\n')
        {
            input[*index] = '\r';
        }
        if (input[*index] == '$')
        {
            while(input[*index] != '\"')
            {
                *index += 1;
            }
            struct token *to_add = init_token(T_WORD, T_EXPAND,
                cut(input, &tmp, *index, len));
            add_token(lexer, to_add);
            break;
        }
        *index += 1;
    }
    if (input[tmp] == '\"')
    {
        shift(input, *index, len);
    }
    return 1;
}

void unquote(char *to_unquote)
{
    size_t index = 0;
    while (index < strlen(to_unquote))
    {
        if (to_unquote[index] == '\'')
        {
            unquote_squotes(to_unquote, &index, strlen(to_unquote));
        }
        else if (to_unquote[index] == '\\')
        {
            unquote_backslashes(to_unquote, &index, index + 2);
        }
        else if (to_unquote[index] == '\"')
        {
            unquote_dquotes(to_unquote, &index, strlen(to_unquote));
        }
    }
}

