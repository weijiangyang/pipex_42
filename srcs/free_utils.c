#include "pipex.h"

// 释放字符串数组（例如由 ft_split() 返回的结果）
void ft_free_split(char **str)
{
    int i;

    i = 0;
    if (!str)
        return;  // 如果传入的是 NULL，直接返回

    // 循环释放每一个字符串元素
    while (str[i])
    {
        free(str[i]);  // 释放 str[i] 指向的单个字符串内存
        i++;
    }

    // 最后释放整个字符串数组本身
    free(str);
}

// 释放 t_cmd 结构体及其成员内存
void ft_free_cmd(t_cmd *cmd)
{
    if (!cmd)
        return;  // 如果传入 NULL，直接返回，避免非法访问

    // 如果 argv 不为空，释放命令参数数组（通常是 execve 使用的参数列表）
    if (cmd->argv)
        ft_free_split(cmd->argv);

    // 如果命令路径存在（例如 "/usr/bin/ls"），释放它
    if (cmd->cmd_path)
        free(cmd->cmd_path);

    // 最后释放整个结构体本身
    free(cmd);
}
