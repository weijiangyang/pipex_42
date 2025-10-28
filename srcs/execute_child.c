#include "pipex.h"

// 子进程1：负责执行第一个命令
void execute_child1(int pipe_fd[2], char **argv, char **envp)
{
    t_cmd *cmd1;         // 保存第一个命令的信息（命令路径、参数等）
    t_status status;     // 用于接收命令初始化状态
    int infile_fd;       // 输入文件描述符

    // 打开输入文件 argv[1]
    infile_fd = open(argv[1], O_RDONLY);
    if (infile_fd < 0)
        handle_invalid_fd(pipe_fd, argv[1]);   // 打开失败则处理错误

    // 初始化第一个命令结构体（解析 argv[2]）
    cmd1 = init_cmd(argv[2], envp, &status);
    if (status != 0)
        handle_invalid_cmd(cmd1, pipe_fd, infile_fd, status); // 命令解析失败时处理

    // 将输入文件重定向到标准输入 (stdin)
    if (dup2(infile_fd, 0) == -1)
        handle_dup2_error(cmd1, pipe_fd, infile_fd);

    // 将管道的写端重定向到标准输出 (stdout)
    if (dup2(pipe_fd[1], 1) == -1)
        handle_dup2_error(cmd1, pipe_fd, infile_fd);

    // 关闭不再使用的文件描述符
    close(pipe_fd[0]);   // 子进程1 不读取管道
    close(pipe_fd[1]);   // 已经重定向到stdout，不再需要原始fd
    close(infile_fd);    // 输入文件已被dup2

    // 执行命令（例如 execve("/bin/ls", ["ls", "-l"], envp)）
    execve(cmd1->cmd_path, cmd1->argv, envp);

    // execve 失败时会执行以下代码
    perror(cmd1->argv[0]);   // 打印错误信息
    ft_free_cmd(cmd1);       // 释放命令结构体内存
    exit(1);                 // 退出子进程
}

// 子进程2：负责执行第二个命令
void execute_child2(int pipe_fd[2], char **argv, char **envp)
{
    t_cmd *cmd2;         // 保存第二个命令的信息
    t_status status;     // 命令解析状态
    int outfile_fd;      // 输出文件描述符

    // 打开输出文件 argv[4]，如果不存在则创建（权限 0644）
    outfile_fd = open(argv[4], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (outfile_fd < 0)
        handle_invalid_fd(pipe_fd, argv[4]);   // 打开失败处理

    // 初始化第二个命令结构体（解析 argv[3]）
    cmd2 = init_cmd(argv[3], envp, &status);
    if (status != 0)
        handle_invalid_cmd(cmd2, pipe_fd, outfile_fd, status);

    // 将管道的读端重定向到标准输入 (stdin)
    if (dup2(pipe_fd[0], 0) == -1)
        handle_dup2_error(cmd2, pipe_fd, outfile_fd);

    // 将输出文件重定向到标准输出 (stdout)
    if (dup2(outfile_fd, 1) == -1)
        handle_dup2_error(cmd2, pipe_fd, outfile_fd);

    // 关闭不再使用的文件描述符
    close(pipe_fd[0]);   // 已经重定向到stdin
    close(pipe_fd[1]);   // 子进程2 不写入管道
    close(outfile_fd);   // 输出文件已被dup2

    // 执行命令（例如 execve("/usr/bin/grep", ["grep", "txt"], envp)）
    execve(cmd2->cmd_path, cmd2->argv, envp);

    // execve 失败时执行以下代码
    perror(cmd2->argv[0]);
    ft_free_cmd(cmd2);
    exit(1);
}
