#include "pipex.h"


// 判断命令字符串是否为空或仅由空白字符组成
int is_empty_or_space(char *argv)
{
    int i;

    i = 0;
    // 如果字符串为空指针或长度为0，直接返回1（表示“是空的”）
    if (!argv || !*argv)
        return (1);

    // 遍历整个字符串
    while (argv[i])
    {
        // 如果遇到任何非空白字符（非空格且非制表符等）
        // 就返回0（表示“不是空的”）
        if (!(argv[i] == ' ' || (argv[i] >= 9 && argv[i] <= 13)))
            return (0);
        i++;
    }

    // 如果循环结束还没返回，说明全是空白字符
    return (1);
}

// 为仅包含空格或空字符串的命令创建一个最简 argv 数组
char **argv_for_empty_space(char *argv)
{
    char **cmd_argv;

    // 分配一个长度为2的字符串数组：
    //   cmd_argv[0] 保存命令本身
    //   cmd_argv[1] = NULL 作为结尾（execve要求以NULL结尾）
    cmd_argv = malloc(sizeof(char *) * 2);
    if (!cmd_argv)
        return (NULL);  // 分配失败，返回NULL

    // 复制 argv 内容（即使是空字符串或只有空格）
    cmd_argv[0] = ft_strdup(argv);
    if (!cmd_argv[0])
        return (free(cmd_argv), NULL);  // 如果复制失败，释放并返回NULL

    // 以NULL结尾，形成合法的 execve 参数格式
    cmd_argv[1] = NULL;

    return (cmd_argv);
}
