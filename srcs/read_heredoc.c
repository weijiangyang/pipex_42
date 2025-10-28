
#include "pipex.h"
#include "get_next_line.h"

void read_here_doc_helper(char *limiter, int *here_doc_fd)
{
    char    *line;

    while (1) // 无限循环读取用户输入
    {
        write(1, "> ", 2); // 提示用户输入
        line = get_next_line(0, 0); // 从 stdin 读取一行输入
        if (!line) // 读到 EOF 或发生错误
            break ;

        if (ft_strcmp(line, limiter) == 0) // 用户输入等于 limiter → 结束输入
        {
            free(line);                // 释放当前行内存
            get_next_line(0, 1);       // 释放 get_next_line 静态缓存
            break ;                     // 退出循环
        }

        write(here_doc_fd[1], line, ft_strlen(line)); // 将输入写入管道写端
        free(line);                                   // 释放当前行内存
    }

    free(limiter);           // 释放 limiter 内存
    close(here_doc_fd[1]);   // 关闭管道写端，让读端收到 EOF
    here_doc_fd[1] = -1;     // 避免重复关闭
}

void read_here_doc(int *here_doc_fd, char **argv)
{
    char    *limiter;

    limiter = ft_strjoin(argv[2], "\n"); // 给 limiter 添加换行符，与 get_next_line 返回值匹配
    if (!limiter)                          // 内存分配失败
        return ;

    if (pipe(here_doc_fd) == -1)           // 创建管道
    {
        free(limiter);                     // 管道创建失败 → 释放内存
        return ;
    }

    read_here_doc_helper(limiter, here_doc_fd); // 调用 helper 循环读取输入并写入管道
}


void close_heredoc_fd(int *here_doc_fd)
{
    if (here_doc_fd[0] >= 0)   // 如果读端有效
        close(here_doc_fd[0]); // 关闭读端
    if (here_doc_fd[1] >= 0)   // 如果写端有效
        close(here_doc_fd[1]); // 关闭写端
}
