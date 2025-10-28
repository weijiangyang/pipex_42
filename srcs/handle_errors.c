#include "pipex.h"

// 处理 pipe() 系统调用失败的情况
int handle_pipe_error(void)
{
    perror("pipe");   // 打印错误信息，例如 "pipe: Too many open files"
    return (1);       // 返回错误码 1（通常供主函数使用）
}

// 处理 fork() 系统调用失败的情况
int handle_fork_error(void)
{
    perror("fork");   // 打印错误信息，例如 "fork: Resource temporarily unavailable"
    return (1);       // 返回错误码 1
}

// 处理 dup2() 系统调用失败的情况
void handle_dup2_error(t_cmd *cmd, int pipe_fd[2], int fd)
{
    // 关闭管道两端，避免文件描述符泄漏
    if (pipe_fd[0] >= 0)
        close(pipe_fd[0]);
    if (pipe_fd[1] >= 0)
        close(pipe_fd[1]);
    if (fd >= 0)
        close(fd);          // 关闭额外传入的文件描述符（输入或输出文件）

    perror("dup2");          // 打印系统错误信息
    ft_free_cmd(cmd);        // 释放命令结构体内存
    exit(1);                 // 立即退出进程（通常是子进程）
}

// 处理打开文件失败的情况（如 infile 或 outfile 无法打开）
void handle_invalid_fd(int pipe_fd[2], char *argv)
{
    // 安全关闭所有管道描述符
    if (pipe_fd[0] >= 0)
        close(pipe_fd[0]);
    if (pipe_fd[1] >= 0)
        close(pipe_fd[1]);

    perror(argv);            // 打印文件名对应的错误信息（例如 "input.txt: No such file or directory"）
    exit(1);                 // 退出子进程
}

// 打印与命令相关的错误信息（不退出，仅输出）
void ft_print_error(t_cmd *cmd, t_status status)
{
    ft_putstr_fd("pipex: ", 2);        // 输出程序名前缀到标准错误
    ft_putstr_fd(cmd->argv[0], 2);     // 输出命令名，例如 "ls"

    // 根据状态码输出不同错误信息
    if (status == CMD_NOT_FOUND)
    {
        // 如果命令中包含路径（含‘/’），说明找不到该文件
        if (ft_strchr(cmd->argv[0], '/'))
            ft_putstr_fd(": No such file or directory", 2);
        else
            ft_putstr_fd(": Command not found", 2);
    }
    else if (status == CMD_MALLOC_ERROR)
        ft_putstr_fd(": memory allocation failed", 2);
    else if (status == CMD_PE_DENIED)
        ft_putstr_fd(": permission denied", 2);

    ft_putstr_fd("\n", 2);  // 换行
}

// 处理命令无效的情况（如找不到命令、无执行权限、内存分配失败）
void handle_invalid_cmd(t_cmd *cmd, int pipe_fd[2], int fd, t_status status)
{
    // 安全关闭打开的文件描述符
    if (pipe_fd[0] >= 0)
        close(pipe_fd[0]);
    if (pipe_fd[1] >= 0)
        close(pipe_fd[1]);
    if (fd >= 0)
        close(fd);

    // 如果命令结构存在，打印对应错误信息
    if (cmd)
        ft_print_error(cmd, status);

    // 释放命令结构体内存
    ft_free_cmd(cmd);

    // 根据错误类型退出进程，返回不同的错误码
    if (status == CMD_NOT_FOUND)
        exit(CMD_PE_DENIED);
    else if (status == CMD_PE_DENIED)
        exit(CMD_PE_DENIED);
    else if (status == CMD_MALLOC_ERROR)
        exit(CMD_MALLOC_ERROR);
}
