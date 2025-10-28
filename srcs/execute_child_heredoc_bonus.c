#include "pipex.h"
#include "get_next_line.h"

/**
 * 总结 here_doc 版本执行流程

读取 here_doc 输入：

read_here_doc 将用户输入写入 here_doc_fd 管道

循环 fork 子进程：

execute_process_loop_heredoc 循环处理每条命令

管道重定向规则：

第一个命令：stdin ← here_doc 管道，stdout → 滚动管道

中间命令：stdin ← 上一管道读端，stdout → 当前管道写端

最后命令：stdin ← 上一管道读端，stdout → 输出文件（追加模式）

关闭管道：

子进程关闭不需要的管道和 here_doc 管道

父进程循环关闭上一条管道，防止 fd 泄漏

错误处理：

命令无效、dup2 失败、文件打开失败，统一调用 bonus 错误处理函数
 */
// 根据命令在循环中的位置调用对应的子进程执行函数
void execute_child_process_heredoc(int *here_doc_fd, t_exe_list *elist,
                char **argv, char **envp)
{
    if (elist->i == 0) // 第一个命令
        execute_first_child_heredoc(here_doc_fd, elist, argv, envp);
    else if (elist->i == elist->cmd_count - 1) // 最后一个命令
        execute_last_child_heredoc(here_doc_fd, elist, argv, envp);
    else // 中间命令
        execute_middle_child_heredoc(here_doc_fd, elist, argv, envp);
}

void execute_process_loop_heredoc(t_exe_list *elist,
                char **argv, char **envp)
{
    int i;
    int here_doc_fd[2];

    read_here_doc(here_doc_fd, argv); // 读取 here_doc 输入并写入管道
    init_pipe_fd(elist);              // 初始化滚动管道
    i = 0;
    while (i < elist->cmd_count)
    {
        elist->i = i;                 // 当前命令索引

        // 为当前命令创建管道（非最后一个命令）
        if (open_pipe_fd_bonus(elist) == 1)
            return ;

        // fork 子进程
        if (fork_child_process(elist) == 1)
            return ;

        if (!elist->pid) // 子进程执行命令
            execute_child_process_heredoc(here_doc_fd, elist, argv, envp);

        close_pipe_parent_process(elist); // 父进程关闭上一条管道
        i++;
    }
    close(here_doc_fd[0]); // 循环结束后关闭 here_doc 管道读端
}
void execute_first_child_heredoc(int *here_doc_fd, t_exe_list *elist,
            char **argv, char **envp)
{
    t_cmd *cmd1;
    t_status cmd1_status;

    // 初始化第一个命令
    cmd1 = init_cmd(argv[3], envp, &cmd1_status);
    if (cmd1_status != 0)
    {
        close_heredoc_fd(here_doc_fd); // 关闭 here_doc 管道
        handle_invalid_cmd_bonus(elist, -1, cmd1, cmd1_status);
    }

    // stdin 重定向到 here_doc 管道读端
    if (dup2(here_doc_fd[0], 0) == -1)
    {
        close_heredoc_fd(here_doc_fd);
        handle_dup2_error_bonus(elist, -1, cmd1);
    }

    // stdout 重定向到滚动管道写端
    if (dup2(elist->pipe_fd[0][1], 1) == -1)
    {
        close_heredoc_fd(here_doc_fd);
        handle_dup2_error_bonus(elist, -1, cmd1);
    }

    close_pipes_child_process(elist); // 关闭所有管道
    close_heredoc_fd(here_doc_fd);    // 关闭 here_doc 管道
    execve(cmd1->cmd_path, cmd1->argv, envp); // 执行命令
    perror(cmd1->argv[0]);
    ft_free_cmd(cmd1);
    exit(1); // 出错退出
}
void execute_last_child_heredoc(int *here_doc_fd, t_exe_list *elist,
                    char **argv, char **envp)
{
    t_cmd *cmd2;
    int outfile_fd;
    t_status cmd2_status;

    close_heredoc_fd(here_doc_fd); // 关闭 here_doc 管道，已不需要

    // 打开输出文件，使用追加模式（heredoc 特性）
    outfile_fd = open(argv[elist->i + 4], O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (outfile_fd < 0)
        handle_invalid_fd_bonus(elist, argv[elist->i + 4]);

    cmd2 = init_cmd(argv[elist->i + 3], envp, &cmd2_status); // 初始化命令
    if (cmd2_status != 0)
        handle_invalid_cmd_bonus(elist, outfile_fd, cmd2, cmd2_status);

    // stdin 重定向到上一条管道读端
    if (dup2(elist->pipe_fd[(elist->i - 1) % 2][0], 0) == -1)
        handle_dup2_error_bonus(elist, outfile_fd, cmd2);

    // stdout 重定向到输出文件
    if (dup2(outfile_fd, 1) == -1)
        handle_dup2_error_bonus(elist, outfile_fd, cmd2);

    close_pipes_child_process(elist); // 关闭所有管道
    close(outfile_fd);                // 关闭文件描述符
    execve(cmd2->cmd_path, cmd2->argv, envp);
    perror(cmd2->argv[0]);
    ft_free_cmd(cmd2);
    exit(1);
}
void execute_middle_child_heredoc(int *here_doc_fd, t_exe_list *elist,
                    char **argv, char **envp)
{
    t_cmd *cmd;
    t_status cmd_status;

    close_heredoc_fd(here_doc_fd); // 中间命令不需要 here_doc 管道

    cmd = init_cmd(argv[elist->i + 3], envp, &cmd_status); // 初始化命令
    if (cmd_status != 0)
        handle_invalid_cmd_bonus(elist, -1, cmd, cmd_status);

    // stdin 重定向到上一条管道读端
    if (dup2(elist->pipe_fd[(elist->i - 1) % 2][0], 0) == -1)
        handle_dup2_error_bonus(elist, -1, cmd);

    // stdout 重定向到当前管道写端
    if (dup2(elist->pipe_fd[(elist->i) % 2][1], 1) == -1)
        handle_dup2_error_bonus(elist, -1, cmd);

    close_pipes_child_process(elist); // 关闭所有管道
    execve(cmd->cmd_path, cmd->argv, envp); // 执行命令
    perror(cmd->argv[0]);
    ft_free_cmd(cmd);
    exit(1);
}
