#include "pipex.h"

/**
 * 作用：初始化 t_exe_list 中的管道数组

	为什么初始化为 -1：

	表示管道端口尚未创建

	方便后续关闭时检查是否需要 close

	pipe_fd 结构：

	pipe_fd[0]、pipe_fd[1] → 两条交替使用的滚动管道

	[0] → 读端，[1] → 写端
 */
void init_pipe_fd(t_exe_list *elist)
{
	elist->pipe_fd[0][0] = -1;
	elist->pipe_fd[0][1] = -1;
	elist->pipe_fd[1][0] = -1;
	elist->pipe_fd[1][1] = -1;
}

/*作用：为当前命令创建一条管道（非最后一个命令）

滚动管道 % 2：

i % 2 交替使用 pipe_fd[0] 和 pipe_fd[1]

避免为每个命令都创建新管道，节省资源

最后一个命令不需要创建管道，因为 stdout 输出到文件*/

int open_pipe_fd_bonus(t_exe_list *elist)
{
    if (elist->i != elist->cmd_count - 1) // 如果不是最后一个命令
    {
        if (pipe(elist->pipe_fd[elist->i % 2]) == -1) // 创建管道
        {
            perror("pipe"); // 创建失败，打印错误
            return (1);     // 返回错误码
        }
    }
    return (0); // 成功
}

/*作用：父进程在 fork 后，关闭上一条管道，避免 文件描述符泄漏

逻辑：

i-1 % 2 → 上一次使用的滚动管道

如果管道端口有效（≥0） → 关闭并标记为 -1

为什么只关闭上一条管道：

当前命令可能还在使用当前管道，不可以关闭*/

void close_pipe_parent_process(t_exe_list *elist)
{
    if (elist->i > 0) // 如果已经创建了上一条管道
    {
        if (elist->pipe_fd[(elist->i - 1) % 2][0] >= 0)
        {
            close(elist->pipe_fd[(elist->i - 1) % 2][0]); // 关闭上一条管道读端
            elist->pipe_fd[(elist->i - 1) % 2][0] = -1;
        }
        if (elist->pipe_fd[(elist->i - 1) % 2][1] >= 0)
        {
            close(elist->pipe_fd[(elist->i - 1) % 2][1]); // 关闭上一条管道写端
            elist->pipe_fd[(elist->i - 1) % 2][1] = -1;
        }
    }
}

/*作用：创建一个子进程执行命令

返回值：

0 → 成功（子进程或父进程都继续执行）

1 → fork 失败

pid 用途：

父进程记录子进程 pid，用于 waitpid 等操作

子进程 pid = 0，用于区分父子进程逻辑*/
int fork_child_process(t_exe_list *elist)
{
    elist->pid = fork(); // 创建子进程
    if (elist->pid == -1) // fork 失败
    {
        handle_pipe_error(); // 打印错误
        return (1);          // 返回错误码
    }
    return (0); // 成功
}
