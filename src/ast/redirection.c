#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "header/astconvert.h"

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
