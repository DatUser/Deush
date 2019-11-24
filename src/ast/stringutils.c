#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*!
**  Transforms int in string
**  \param value : number
**  \param s : empty string already malloc
**  \return fulfilled string
**/
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

/*!
**  Parses the line received by get_next_line
**  \param char *line : string to parse
**  \return char **s : array of char* (the words in line)
**/
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

/*!
**  Extract the IO numbers for redirection
**  \param s : redirection string
**  \return extracted number
**/
int extract_nb(char *s)
{
    size_t i = 0;
    while (s[i] && (s[i] >= '0' && s[i] <= '9'))
        i++;

    char save = s[i];
    s[i] = '0';

    int out = atoi(s);
    s[i] = save;

    return out;
}
