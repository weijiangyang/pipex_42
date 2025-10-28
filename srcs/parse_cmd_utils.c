#include "pipex.h"

// 将命令名与指定的路径拼接成完整命令路径
// 例如：path="/usr/bin", argv="ls" -> 返回 "/usr/bin/ls"
char *get_path_helper(char *argv, char *path)
{
    char *tmp;       // 临时字符串，用于存储 path + "/"
    char *cmd_path;  // 最终拼接的完整命令路径

    // 在路径末尾加上斜杠 "/"
    tmp = ft_strjoin(path, "/");
    if (!tmp)
        return (NULL);   // 分配失败，返回NULL

    // 拼接命令名 argv
    cmd_path = ft_strjoin(tmp, argv);
    if (!cmd_path)
        return (NULL);   // 分配失败，返回NULL

    free(tmp);           // 释放临时字符串内存
    return (cmd_path);   // 返回完整路径字符串
}

// 将命令字符串拆分成 argv 数组
// 例如：argv="ls -l -a" -> 返回 ["ls", "-l", "-a", NULL]
char **get_argv(char *argv)
{
    char **argv_cmd;

    // 使用空格分割字符串，生成 argv 数组
    argv_cmd = ft_split(argv, ' ');
    if (!argv_cmd)
        return (NULL);   // 分配失败，返回 NULL

    return (argv_cmd);
}
