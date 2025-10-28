#include "pipex.h"
#include "get_next_line.h"

// -----------------------------
// 读取用户输入的 here_doc 内容，并写入管道
// limiter: here_doc 的结束标识符（用户输入这一行结束）
// here_doc_fd: 管道数组，写端写入用户输入
// -----------------------------
void read_here_doc_helper(char *limiter, int *here_doc_fd)
{
    char *line;

    while (1)
    {
        write(1, "> ", 2);                 // 打印提示符，提示用户输入
        line = get_next_line(0, 0);        // 从 stdin 读取一行
        if (!line)                         // EOF 或读取失败
            break ;
        if (ft_strcmp(line, limiter) == 0) // 用户输入结束标识符
        {
            free(line);                     // 释放当前行内存
            get_next_line(0, 1);            // 释放 get_next_line 静态缓存
            break ;                         // 停止读取
        }
        write(here_doc_fd[1], line, ft_strlen(line)); // 将用户输入写入管道
        free(line);                        // 释放内存
    }

    free(limiter);                        // 释放 limiter 内存
    close(here_doc_fd[1]);                // 关闭管道写端，子进程可读取 EOF
    here_doc_fd[1] = -1;                  // 防止重复关闭
}

// -----------------------------
// 初始化 here_doc 管道并读取内容
// here_doc_fd: int[2] 管道数组
// argv: 命令行参数，argv[2] 为 LIMITER
// -----------------------------
void read_here_doc(int *here_doc_fd, char **argv)
{
    char *limiter;

    limiter = ft_strjoin(argv[2], "\n"); // 拼接换行符，因为 get_next_line 返回带换行符的行
    if (!limiter)                         // 内存分配失败
        return ;
    
    if (pipe(here_doc_fd) == -1)          // 创建管道失败
    {
        free(limiter);
        return ;
    }

    read_here_doc_helper(limiter, here_doc_fd); // 调用 helper 读取输入写入管道
}

// -----------------------------
// 安全关闭 here_doc 管道
// -----------------------------
void close_heredoc_fd(int *here_doc_fd)
{
    if (here_doc_fd[0] >= 0)             // 如果读端未关闭
        close(here_doc_fd[0]);
    if (here_doc_fd[1] >= 0)             // 如果写端未关闭
        close(here_doc_fd[1]);
}
