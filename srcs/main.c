#include "pipex.h"

int main(int ac, char **av, char **envp)
{
    int pipe_fd[2];
    pid_t pid_1;
    pid_t pid_2;

    if (ac == 5)
    {
        if (pipe(pipe_fd) == -1)
            handle_pipe_error();
        pid_1 = fork();
        if (pid_1 == -1)
            handle_fork_error();
        if (pid_1 == 0)
            execute_child1(pipe_fd, av, envp);
        pid_2 = fork();
        if (pid_2 == -1)
            handle_fork_error();
        if (pid_2 == 0)
            execute_child2(pipe_fd, av, envp);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return (handle_waitpid(pid_2));
    }
    else
        ft_putstr_fd("Usage: ./pipex infile cmd1 cmd2 outfile\n", 2);
}