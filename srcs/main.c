#include "pipex.h"

#include "pipex.h"

/**
 * 程序入口函数
 * 作用：模拟 shell 命令
 *      < infile cmd1 | cmd2 > outfile
 * 即：
 *   - 从 infile 读取输入；
 *   - 把 cmd1 的输出通过管道传给 cmd2；
 *   - 把 cmd2 的输出重定向到 outfile。
 */
int main(int ac, char **av, char **envp)
{
    int pipe_fd[2];     // pipe_fd[0]：读端； pipe_fd[1]：写端
    pid_t pid_1;        // 第一个子进程（执行 cmd1）
    pid_t pid_2;        // 第二个子进程（执行 cmd2）

    // 检查命令行参数数量是否正确
    // ./pipex infile cmd1 cmd2 outfile  → 共 5 个参数
    if (ac == 5)
    {
        // 创建一个管道（pipe_fd）
        // pipe_fd[0] 用于读，pipe_fd[1] 用于写
        if (pipe(pipe_fd) == -1)
            handle_pipe_error();

        // 创建第一个子进程
        pid_1 = fork();
        if (pid_1 == -1)
            handle_fork_error();

        // 子进程1：执行 cmd1
        // 它的标准输入从 infile 来，标准输出写入管道写端
        if (pid_1 == 0)
            execute_child1(pipe_fd, av, envp);

        // 创建第二个子进程
        pid_2 = fork();
        if (pid_2 == -1)
            handle_fork_error();

        // 子进程2：执行 cmd2
        // 它的标准输入从管道读端来，标准输出重定向到 outfile
        if (pid_2 == 0)
            execute_child2(pipe_fd, av, envp);

        // 父进程：关闭所有 pipe 文件描述符（不再使用）
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        // 等待子进程结束，并返回最后一个命令的退出状态
        return (handle_waitpid(pid_2));
    }
    else
        // 参数错误时提示正确用法
        ft_putstr_fd("Usage: ./pipex infile cmd1 cmd2 outfile\n", 2);
}
