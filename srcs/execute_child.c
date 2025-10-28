#include "pipex.h"

void execute_child1(int pipe_fd[2], char **argv, char **envp)
{
    t_cmd *cmd1;
    t_status status;
    int infile_fd;

    infile_fd = open(argv[1], O_RDONLY);
    if (infile_fd < 0)
        handle_invalid_fd(pipe_fd, argv[1]);
    cmd1 = init_cmd(argv[2], envp, &status);
    if (status != 0)
        handle_invalid_cmd(cmd1, pipe_fd, infile_fd, status);
    if (dup2(infile_fd, 0) == -1)
        handle_dup2_error(cmd1, pipe_fd, infile_fd);

    if (dup2(pipe_fd[1], 1) == -1)
        handle_dup2_error(cmd1, pipe_fd, infile_fd);
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    close(infile_fd);
    execve(cmd1->cmd_path, cmd1->argv, envp);
    perror(cmd1->argv[0]);
    ft_free_cmd(cmd1);
    exit(1);
}

void execute_child2(int pipe_fd[2], char **argv, char **envp)
{
    t_cmd *cmd2;
    t_status status;
    int outfile_fd;

    outfile_fd = open(argv[4], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (outfile_fd < 0)
        handle_invalid_fd(pipe_fd, argv[4]);
    cmd2 = init_cmd(argv[3], envp, &status);
    if (status != 0)
        handle_invalid_cmd(cmd2, pipe_fd, outfile_fd, status);
    if (dup2(pipe_fd[0], 0) == -1)
        handle_dup2_error(cmd2, pipe_fd, outfile_fd);
    if (dup2(outfile_fd, 1) == -1)
        handle_dup2_error(cmd2, pipe_fd, outfile_fd);
    close (pipe_fd[0]);
    close(pipe_fd[1]);
    close(outfile_fd);
    execve(cmd2->cmd_path, cmd2->argv, envp);
    perror(cmd2->argv[0]);
    ft_free_cmd(cmd2);
    exit(1);
}