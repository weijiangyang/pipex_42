#include "pipex.h"

/**
 * 🔑 总结

参数检查：

普通模式至少 2 条命令 + 输入文件 + 输出文件

here_doc 模式至少 2 条命令 + LIMITER + 输出文件

t_exe_list 初始化：

here_doc_mode 表示是否是 here_doc

cmd_count 保存命令数量，用于循环 fork 子进程

执行循环：

根据 here_doc_mode 调用对应的子进程执行循环

子进程通过滚动管道实现多命令连接

等待子进程结束：

handle_plural_waitpid 获取最后一个命令的退出码并返回

错误提示：

参数不够或命令不足时打印对应提示
 * 
 */
// 打印 here_doc 模式错误提示
void ft_print_error_here_doc_msg(void)
{
    ft_putstr_fd("Error: here_doc mode accepts at least 2 commands\n", 2);
    ft_putstr_fd("Usage: ./pipex here_doc LIMITER cmd1 ... cmdn outfile\n", 2);
}

// 打印普通模式错误提示
void ft_print_error_normal_mode_msg(void)
{
    ft_putstr_fd("Usage: ./pipex infile cmd1 cmd2 ... cmdn outfile\n", 2);
}

// 检查 argv 是否为 here_doc 模式，并检查参数数量
int check_argv_here_doc(int argc, char **argv)
{
    if (ft_strcmp("here_doc", argv[1]) == 0) // 第一个参数是 "here_doc"
    {
        if (argc >= 6) // 至少需要 LIMITER + 2 个命令 + outfile
            return (0); // 参数合法
        if (argc == 5) // 只有 1 个命令，不够
        {
            ft_print_error_here_doc_msg();
            return (1); // 参数错误
        }
    }
    return (0); // 非 here_doc 模式
}

// 初始化 t_exe_list 结构体
void init_elist(int argc, char **argv, t_exe_list *elist)
{
    // 判断是否是 here_doc 模式
    if (ft_strcmp(argv[1], "here_doc") == 0)
        elist->here_doc_mode = 1;
    else
        elist->here_doc_mode = 0;

    // 计算命令数量：
    // 普通模式：argc - infile - outfile - 1 = argc - 3
    // here_doc 模式：还要减去 LIMITER，故 -1
    elist->cmd_count = argc - 3 - elist->here_doc_mode;
}

// 主函数
int main(int argc, char **argv, char **envp)
{
    t_exe_list elist;
    int last_exit_code;

    if (argc >= 5) // 至少需要 infile + 2 个命令 + outfile 或 here_doc + LIMITER + 2 cmd + outfile
    {
        if (check_argv_here_doc(argc, argv)) // 参数不合法，打印错误
            return (1);

        init_elist(argc, argv, &elist); // 初始化执行列表结构体

        // 根据模式执行不同循环
        if (elist.here_doc_mode)
            execute_process_loop_heredoc(&elist, argv, envp); // here_doc 模式
        else
            execute_process_loop(&elist, argv, envp);          // 普通模式

        // 等待所有子进程结束，并获取最后一个命令的退出码
        last_exit_code = handle_plural_waitpid(elist.pid, elist.cmd_count);
        return (last_exit_code);
    }
    else // 参数数量不足
    {
        if (argv[1] && ft_strcmp(argv[1], "here_doc") == 0)
            ft_print_error_here_doc_msg(); // here_doc 模式错误提示
        else
            ft_print_error_normal_mode_msg(); // 普通模式错误提示
    }   
    return (1); // 返回错误
}
