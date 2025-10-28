#include "pipex.h"

int handle_waitpid(pid_t pid_2)
{
    int status;
    int i;
    int cmd_count;
    int exit_code;
    pid_t wpid;

    i = 0;
    cmd_count = 2;
    while (i < cmd_count)
    {
        wpid = waitpid(-1, &status, 0);
        if (wpid == -1)
        {
            if (errno !=  ECHILD)
                break;
            perror("waitpid");
            break;
        }
        if (wpid == pid_2)
            exit_code = get_last_exit_code(status);
        i++;
    }
    return (exit_code);
}

int handle_plural_waitpid(pid_t pid, int cmd_count)
{
    int status;
    int i;
    int exit_code;
    pid_t wpid;

    i = 0;
    while (i < cmd_count)
    {
        wpid = waitpid(-1, &status, 0);
        if (wpid == -1)
        {
            if (errno != ECHILD)
                break;
            perror("waitpid");
            break;
        }
        if(wpid == pid)
            exit_code = get_last_exit_code(status);
        i++;
    }
    return (exit_code);
}

int get_last_exit_code(int status)
{
    int exit_code;
    int signal;

    if (WIFEXITED(status))
        exit_code = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
    {
        signal = WTERMSIG(status);
        exit_code = 128 + signal;
    }
    else
        exit_code = -1;
    return (exit_code);
}