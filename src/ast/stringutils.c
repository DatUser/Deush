#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *my_itoa(int value, char *s)
{
    int start = 0;
    int len = 0;

    if (value == 0)
    {
        s[0] = '0';
        s[1] = '\0';
        return s;
    }
    else if (value < 0)
    {
        s[0] = '-';
        start = 1;
        len = 1;
        value *= -1;
    }

    int copy = value;
    while (copy > 0)
    {
        len++;
        copy /= 10;
    }

    for (int i = len - 1; i >= start; i--)
    {
        char c = value % 10 + '0';
        s[i] = c;
        value /= 10;
    }

    s[len] = '\0';

    return s;
}

char *str_replace_acolade(char *cmd, char *arg, int *i)
{
    size_t lenfirst = strlen(cmd) - 2;
    size_t lensecond = strlen(arg);
    size_t len = lenfirst + lensecond + 1;

    cmd[*i] = '\0';
    cmd[*i + 1] = '\0';

    char *new = malloc(len);
    new = strcpy(new, cmd);
    new = strcat(new, arg);
    new = strcat(new, cmd + *i + 2);

    *i = strlen(cmd) + lensecond;

    free(cmd);

    return new;
}

char *str_concat_space(char *first, char *second)
{
    size_t lenfirst = strlen(first);
    size_t lensecond = strlen(second);
    size_t len = lenfirst + lensecond + 2;

    first = realloc(first, len);
    first = strcat(first, " ");
    first = strcat(first, second);

    return first;
}

/*!
*   Parses the line received by get_next_line
*   \param char *line : string to parse
*   \return char **s : array of char* (the words in line)
*/
char **cut_line(char *s, size_t *tab_len)
{
    //char *s = strdup(line);
    *tab_len = strlen(s);
    char **buff = malloc(sizeof(char*) * *tab_len);

    char *tmp = strtok(s, " ");
    int i = 0;

    while (tmp)
    {
        buff[i] = tmp;
        tmp = strtok(NULL, " ");
        i++;
    }

    buff[i] = NULL;

    return buff;
}
