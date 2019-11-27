#define _GNU_SOURCE
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "header/astconvert.h"
#include "header/stringutils.h"
#include "../prompt/header/prompt.h"
#include "../include/global.h"

/*!
**  Redirect <
**  \param ast : node containing the redirection
**  \param targetfd : the targeted file descriptor
**  \return Execution's return value
**/
int eval_redirect_left(struct ast *ast, int targetfd)
{
    char *child = ast->child->node->child->next->node->data;
    int fd = open(child, O_RDONLY);
    int save_fd = dup(targetfd);//saves the current state of the target fd

    if (fd < 0 || save_fd < 0)
    {
        warnx("file descriptor problem");
        return 1;
    }
    dup2(fd, targetfd);//puts the file in the target fd

    struct ast separator = { ast->type, ast->data, ast->nb_children,
                                ast->child->node->child };
    int out = eval_ast(&separator);

    dup2(save_fd, targetfd);//restores the previous state of target fd

    close(save_fd);
    close(fd);
    return out;
}

/*!
**  Redirect >
**  \param ast : node containing the redirection
**  \param sourcefd : the source file descriptor
**  \return Execution's return value
**/
int eval_redirect_right(struct ast *ast, int sourcefd)
{
    char *child = ast->child->node->child->next->node->data;
    int fd = -1;
    if (ast->child->node->type != T_RGREAT)
    {
        if (access(child, F_OK) < 0)//check if file exists
        {
            fd = open(child, O_WRONLY|O_CREAT,
            S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        }
        else
        {
            fd = open(child, O_WRONLY|O_TRUNC);
        }
    }
    else
    {
        fd = open(child, O_CREAT|O_WRONLY|O_APPEND,
        S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    }

    int save_fd = dup(sourcefd);//saves the current state of the target fd

    if (fd < 0 || save_fd < 0)
    {
        warnx("file descriptor problem");
        return 1;
    }
    dup2(fd, sourcefd);//puts the file in the target fd

    struct ast separator = { ast->type, ast->data, ast->nb_children,
                                ast->child->node->child };
    int out = eval_ast(&separator);

    dup2(save_fd, sourcefd);//restores the previous state of target fd

    close(save_fd);
    close(fd);
    return out;
}

int eval_redirect_both(struct ast *ast, int sourcefd)
{
    char *child = ast->child->node->child->next->node->data;
    int fd = -1;
    if (access(child, F_OK) < 0)//check if file exists
        fd = open(child, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    else
        fd = open(child,O_RDWR);

    int save_fd = dup(sourcefd);//saves the current state of the target fd

    if (fd < 0 || save_fd < 0)
    {
        sourcefd = 0;
        save_fd = dup(sourcefd);//saves the current state of the target fd
    }

    dup2(fd, sourcefd);//puts the file in the target fd

    struct ast separator = { ast->type, ast->data, ast->nb_children,
                                ast->child->node->child };
    int out = eval_ast(&separator);

    dup2(save_fd, sourcefd);//restores the previous state of target fd

    close(save_fd);
    close(fd);
    return out;
}

/*!
**  Redirect <
**  \param ast : node containing the redirection
**  \param targetfd : the targeted file descriptor
**  \return Execution's return value
**/
int eval_redirect_double_left(struct ast *ast, int targetfd)
{
    char *child = ast->child->node->child->next->node->data;
    char *line = NULL;
    char *text = NULL;
    int tmp = open("/tmp", O_TMPFILE|O_RDWR);

    while (strcmp(line = get_next_line(PS2), child))
    {
        text = append(line, text);
        free(line);
    }

    int save = -1;
    if (text && fcntl(targetfd, F_GETFD) >= 0 && tmp > 0)
    {
        write(tmp, text, strlen(text));
        lseek(tmp, 0, SEEK_SET);//sets the offset to 0 in order to be at the
        save = dup(targetfd);   //beginning of the text
        dup2(tmp, targetfd);
    }

    struct ast separator = { ast->type, ast->data, ast->nb_children,
                                ast->child->node->child };
    int out = eval_ast(&separator);

    if (save >= 0)
    {
        dup2(save, targetfd);
        close(save);
    }

    if (tmp > 0)
        close(tmp);

    free(line);
    free(text);
    return out;
}

int eval_redirect_right_and(struct ast *ast)
{
    char *left_child = ast->child->node->data;
    char *right_child = ast->child->node->child->next->node->data;

    int fd_left = extract_nb(left_child);
    int fd_right = extract_nb(right_child);

    if (fd_left && !is_num(right_child))
        return eval_redirect_right(ast, 1);

    int save = dup(fd_right);

    if (save < 0)
    {
        warnx("Bad file descriptor");
        return 1;
    }

    dup2(fd_left, fd_right);

    struct ast separator = { ast->type, ast->data, ast->nb_children,
                                ast->child->node->child };
    int out = eval_ast(&separator);

    dup2(save, fd_right);

    close(save);
    return out;
}
