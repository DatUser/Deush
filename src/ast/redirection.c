#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
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

    if (fd < 0)
        return 1;

    int fd_pipe[2];
    pipe(fd_pipe);
    pid_t left = fork();

    if (left > 0)
    {
        close(fd_pipe[0]);
        //dup(fd[1], 1);//TO WRITE
        char buf[1024];

        while (read(fd, buf, 1024) > 0)
            write(fd_pipe[1], buf, 1024);

        close(fd_pipe[1]);

        int status = 0;
        waitpid(left, &status, 0);

        close(fd);
        return WEXITSTATUS(status);

    }
    else
    {
        close(fd_pipe[1]);
        dup2(fd_pipe[0], targetfd);//TO READ
        close(fd_pipe[0]);
        //printf("left : \n");

        struct ast separator = { ast->type, ast->data, ast->nb_children,
                                ast->child->node->child };
        //separator->child->child->node->child->node;

        exit(eval_ast(&separator/*ast->child->node*/));
    }

    return 0;
}
