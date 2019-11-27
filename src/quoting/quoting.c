#include "../include/include.h"
#include "../include/global.h"
#include "header/quoting.h"

static void shift(char *input, size_t index, size_t len)
{
    while (index < len - 1)
    {
        input[index] = input[index + 1];
        index++;
    }
}

size_t unquote_backslashes(char *input, size_t *index, size_t len)
{
    while (*index < len)
    {
        if (input[*index] == '\\')
        {
            shift(input, *index, len);
            len--;
        }
        *index += 1;
    }
    return len;
}

size_t unquote_squotes(char *input, size_t *index, size_t len)
{
    shift();
    while (*index < len && input[*index] != '\'')
    {
        *index += 1;
    }
    
}

int main(void)
{
    char to_shift[] = "\\a\\\\\\b\\c\\defg";
    printf("%zu\n", unquote_backslashes(to_shift, strlen(to_shift));
    printf("%s\n", to_shift);
}
