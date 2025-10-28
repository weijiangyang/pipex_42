#include "pipex.h"

// 处理所有命令的循环
void execute_process_loop(t_exe_list *elist, char **argv, char **envp)
{
    int i;

    init_pipe_fd(elist); // 初始化滚动管道，所有 fd 置为 -1
    i = 0;
    while (i < elist->cmd_count) // 循环每条命令
    {
        elist->i = i; // 当前命令索引

        // 为当前命令创建管道（非最后一个命令）
        if (open_pipe_fd_bonus(elist) == 1)
            return ;

        // fork 子进程
        if (fork_child_process(elist) == 1)
            return ;

        if (!elist->pid) // 子进程执行逻辑
        {
            if (elist->i == 0) // 第一个命令
                execute_first_child(elist, argv, envp);
            else if (elist->i == elist->cmd_count - 1) // 最后一个命令
                execute_last_child(elist, argv, envp);
            else // 中间命令
                execute_middle_child(elist, argv, envp);
        }

        close_pipe_parent_process(elist); // 父进程关闭上一条管道
        i++;
    }
}

void execute_first_child(t_exe_list *elist, char **argv, char **envp)
{
    t_cmd *cmd1;
    int infile_fd;
    t_status cmd1_status;

    infile_fd = open(argv[1], O_RDONLY); // 打开输入文件
    if (infile_fd < 0)
        handle_invalid_fd_bonus(elist, argv[1]); // 文件打开失败处理

    cmd1 = init_cmd(argv[2], envp, &cmd1_status); // 初始化命令结构
    if (cmd1_status != 0)
        handle_invalid_cmd_bonus(elist, infile_fd, cmd1, cmd1_status); // 命令无效处理

    // stdin 重定向到输入文件
    if (dup2(infile_fd, 0) == -1)
        handle_dup2_error_bonus(elist, infile_fd, cmd1);

    // stdout 重定向到管道写端
    if (dup2(elist->pipe_fd[0][1], 1) == -1)
        handle_dup2_error_bonus(elist, infile_fd, cmd1);

    close_pipes_child_process(elist); // 关闭管道，防止 fd 泄漏
    close(infile_fd);                 // 关闭输入文件 fd

    execve(cmd1->cmd_path, cmd1->argv, envp); // 执行命令
    perror(cmd1->argv[0]);                     // execve 失败打印错误
    ft_free_cmd(cmd1);
    exit(1); // 出错退出
}


void execute_last_child(t_exe_list *elist, char **argv, char **envp)
{
    t_cmd *cmd2;
    int outfile_fd;
    t_status cmd2_status;

    // 打开输出文件，创建或截断
    outfile_fd = open(argv[elist->i + 3], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (outfile_fd < 0)
        handle_invalid_fd_bonus(elist, argv[elist->i + 3]);

    cmd2 = init_cmd(argv[elist->i + 2], envp, &cmd2_status); // 初始化命令
    if (cmd2_status != 0)
        handle_invalid_cmd_bonus(elist, outfile_fd, cmd2, cmd2_status);

    // stdin 重定向到上一条管道读端
    if (dup2(elist->pipe_fd[(elist->i - 1) % 2][0], 0) == -1)
        handle_dup2_error_bonus(elist, outfile_fd, cmd2);

    // stdout 重定向到输出文件
    if (dup2(outfile_fd, 1) == -1)
        handle_dup2_error_bonus(elist, outfile_fd, cmd2);

    close_pipes_child_process(elist); // 关闭所有管道
    close(outfile_fd);                // 关闭输出文件 fd

    execve(cmd2->cmd_path, cmd2->argv, envp); // 执行命令
    perror(cmd2->argv[0]);
    ft_free_cmd(cmd2);
    exit(1);
}


void execute_middle_child(t_exe_list *elist, char **argv, char **envp)
{
    t_cmd *cmd;
    t_status cmd_status;

    cmd = init_cmd(argv[elist->i + 2], envp, &cmd_status); // 初始化命令
    if (cmd_status != 0)
        handle_invalid_cmd_bonus(elist, -1, cmd, cmd_status);

    // stdin 重定向到上一条管道读端
    if (dup2(elist->pipe_fd[(elist->i - 1) % 2][0], 0) == -1)
        handle_dup2_error_bonus(elist, -1, cmd);

    // stdout 重定向到当前管道写端
    if (dup2(elist->pipe_fd[(elist->i) % 2][1], 1) == -1)
        handle_dup2_error_bonus(elist, -1, cmd);

    close_pipes_child_process(elist); // 关闭所有管道，防止 fd 泄漏

    execve(cmd->cmd_path, cmd->argv, envp); // 执行命令
    perror(cmd->argv[0]);
    ft_free_cmd(cmd);
    exit(1);
}
