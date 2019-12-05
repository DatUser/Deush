#define _GNU_SOURCE 
#include <unistd.h>
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
    *tab_len = strlen(s) + 1;
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
    s[i] = '\0';

    int out = atoi(s);
    s[i] = save;

    return out;
}

/*!
**  Concatenates 2 string
**  at the end and realloc and the space needed in dest
**/
char *append_classic(char *src, char *dest)
{
    if (src)
    {
        size_t len = strlen(src) + strlen(dest) + 1;
        dest = realloc(dest, len);
        dest = strcat(dest, src);
    }

    return dest;
}

/*!
**  Concatenates 2 strings and adds a \n
**  at the end and realloc and the space needed in dest
**/
char *append(char *src, char *dest)
{
    if (dest)
    {
        size_t len = strlen(src) + strlen(dest) + 2;
        dest = realloc(dest, len);
        dest = strcat(dest, src);
    }
    else
    {
        size_t len = strlen(src) + 2;
        dest = realloc(dest, len);
        dest = strcpy(dest, src);
    }

    strcat(dest, "\n");
    return dest;
}

int is_num(char *s)
{
    //s is not supposed to be NULL

    size_t i = 0;
    size_t len = strlen(s);
    int is_nb = 1;

    while (is_nb && i < len)
    {
        if (s[i] < '0' || s[i] > '9')
            is_nb = 0;
        i++;
    }

    return is_nb;
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
**  Appends source to dst ignoring repetitive newlines
**/
char *append_no_newline(char *dst, char *src)
{
    //if (!dst)
    //    return strdup(src);

    size_t len_dst = (dst) ? strlen(dst) : 0;
    size_t len_src = strlen(src);

    dst = realloc(dst, len_dst + len_src + 1);
    size_t i = len_dst;
    size_t j = 0;

    while (src[j])
    {
        if (src[j] != '\n' /*|| !src[j+ 1]*/)
            dst[i] = src[j];
        else
            dst[i] = ' ';
        i++;
        j++;
    }

    dst[i] = '\0';

    return dst;
}

/*!
**  Extract string from file ignoring reptitive newlines
**/
char *extract_file(int fd)
{
    lseek(fd, 0, SEEK_SET); 
    ssize_t len = 0;
    char data[512] = { 0 };
    char *output = NULL;

    while ((len = read(fd, data, 511)) > 0)
        output = append_no_newline(output, data); 

    return output;
}
