#include "pipex.h"

// 等待两个子进程结束，并获取第二个子进程的退出码
int handle_waitpid(pid_t pid_2)
{
    int status;        // waitpid 返回的状态信息
    int i;             // 计数器
    int cmd_count;     // 子进程数量，这里固定为 2
    int exit_code;     // 保存 pid_2 的退出码
    pid_t wpid;        // waitpid 返回的子进程 pid

    i = 0;
    cmd_count = 2;
    while (i < cmd_count)
    {
        wpid = waitpid(-1, &status, 0);  // 等待任意子进程结束
        if (wpid == -1)
        {
            if (errno !=  ECHILD)       // 如果不是没有子进程错误
                break;
            perror("waitpid");          // 打印错误信息
            break;
        }

        // 如果是第二个子进程 pid_2，获取退出码
        if (wpid == pid_2)
            exit_code = get_last_exit_code(status);
        i++;
    }
    return (exit_code);
}

// 等待多个子进程结束，并获取指定 pid 的退出码
int handle_plural_waitpid(pid_t pid, int cmd_count)
{
    int status;
    int i;
    int exit_code;
    pid_t wpid;

    i = 0;
    while (i < cmd_count)
    {
        wpid = waitpid(-1, &status, 0);  // 等待任意子进程结束
        if (wpid == -1)
        {
            if (errno != ECHILD)        // 如果不是没有子进程错误
                break;
            perror("waitpid");          // 打印错误信息
            break;
        }

        // 如果是指定 pid，保存退出码
        if (wpid == pid)
            exit_code = get_last_exit_code(status);
        i++;
    }
    return (exit_code);
}

// 根据 waitpid 返回的 status 获取子进程退出码
int get_last_exit_code(int status)
{
    int exit_code;
    int signal;

    // 正常退出
    if (WIFEXITED(status))
        exit_code = WEXITSTATUS(status);  // 获取退出码
    // 被信号终止
    else if (WIFSIGNALED(status))
    {
        signal = WTERMSIG(status);       // 获取终止信号编号
        exit_code = 128 + signal;        // 遵循 shell 约定：128 + 信号编号
    }
    else
        exit_code = -1;                  // 其他情况返回 -1

    return (exit_code);
}
