#include "../include/include.h"
#include "../include/global.h"
#include "header/quoting.h"

size_t unquote_squotes(char *input, size_t *index, size_t len);

int SQUO = 0;
int DQUO = 0;

static void shift(char *input, size_t index, size_t len)
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
        *index += 1;
    }
    return 1;
}

int main(void)
{
    char to_shift[] = "\"lmao\\\"\"'mdr'";
    size_t index = 0;
    while (index < strlen(to_shift))
    {
        printf("%s\n", to_shift);
        if (to_shift[index] == '\'')
        {
            unquote_squotes(to_shift, &index, strlen(to_shift));
        }
        else if (to_shift[index] == '\\')
        {
            unquote_backslashes(to_shift, &index, index + 2);
        }
        else if (to_shift[index] == '\"')
        {
            unquote_dquotes(to_shift, &index, strlen(to_shift));
        }
        else
        {
            index++;
        }
    }
    printf("%s\n", to_shift);
}
