#include "pipex.h"

int handle_pipe_error(void)
{
    perror("pipe");
    return (1);
}

int handle_fork_error(void)
{
    perror("fork");
    return (1);
}

void handle_dup2_error(t_cmd *cmd, int pipe_fd[2], int fd)
{
    if (pipe_fd[0] >= 0)
        close(pipe_fd[0]);
    if (pipe_fd[1] >= 0)
        close(pipe_fd[1]);
    if (fd >= 0)
        close(fd);
    perror("dup2");
    ft_free_cmd(cmd);
    exit(1);
}

void handle_invalid_fd(int pipe_fd[2], char *argv)
{
    if (pipe_fd[0] >= 0)
        close(pipe_fd[0]);
    if (pipe_fd[1] >= 0)
        close(pipe_fd[1]);
    perror(argv);
    exit(1);
}

void ft_print_error(t_cmd *cmd, t_status status)
{
    ft_putstr_fd("pipex: ", 2);
    ft_putstr_fd(cmd->argv[0], 2);
    if (status == CMD_NOT_FOUND)
    {
        if (ft_strchr(cmd->argv[0], '/'))
            ft_putstr_fd(": No such file or directory", 2);
        else
            ft_putstr_fd(": Command not found", 2);
    }
    else if (status == CMD_MALLOC_ERROR)
        ft_putstr_fd(": memory allocation failed", 2);
    else if (status == CMD_PE_DENIED)
        ft_putstr_fd(": permission denied", 2);
    ft_putstr_fd("\n", 2);
}

void handle_invalid_cmd(t_cmd *cmd, int pipe_fd[2], int fd, t_status status)
{
    if (pipe_fd[0] >= 0)
        close(pipe_fd[0]);
    if (pipe_fd[1] >= 0)
        close(pipe_fd[1]);
    if (fd >= 0)
        close(fd);
    if (cmd)
        ft_print_error(cmd, status);
    ft_free_cmd(cmd);
    if (status == CMD_NOT_FOUND)
        exit(CMD_PE_DENIED);
    else if (status == CMD_PE_DENIED)
        exit (CMD_PE_DENIED);
    else if (status == CMD_MALLOC_ERROR)
        exit (CMD_MALLOC_ERROR);
}