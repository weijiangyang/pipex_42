#include "pipex.h"

// 从环境变量 envp 中获取 PATH，并拆分成路径数组
// 成功返回路径数组，失败通过 status 返回错误类型
char **get_path(char **envp, t_status *status)
{
    char **paths;

    // 遍历 envp，找到以 "PATH=" 开头的环境变量
    while (*envp && ft_strncmp("PATH=", *envp, 5))
        envp++;

    // 如果没找到 PATH 环境变量
    if (!*envp)
        return (*status = CMD_NOT_FOUND, NULL);

    // 去掉 "PATH=" 前缀，并用冒号 ':' 拆分成各个路径
    paths = ft_split(*envp + 5, ':');
    if (!paths)
        return (*status = CMD_MALLOC_ERROR, NULL);

    return (paths);
}

// 根据 PATH 路径数组查找命令可执行文件完整路径
char *get_cmd_path(char **argv, char **envp, t_status *status)
{
    char **paths;
    char *cmd_path;
    char **start_paths;

    // 获取 PATH 中的路径数组
    paths = get_path(envp, status);
    if (!paths)
        return (NULL);

    start_paths = paths;  // 保存起始指针，用于后续释放

    // 遍历 PATH 路径，尝试拼接命令完整路径
    while (*paths)
    {
        cmd_path = get_path_helper(argv[0], *paths); // 拼接 "path/command"
        if (!cmd_path)
            return (*status = CMD_MALLOC_ERROR, ft_free_split(start_paths), NULL);

        // 检查文件是否存在且可执行
        if (access(cmd_path, F_OK) == 0 && access(cmd_path, X_OK) == 0)
            return (*status = CMD_SUCCESS, ft_free_split(start_paths), cmd_path);

        // 文件存在但不可执行
        else if (access(cmd_path, F_OK) == 0 && access(cmd_path, X_OK) != 0)
            return (*status = CMD_PE_DENIED, ft_free_split(start_paths), NULL);

        free(cmd_path);  // 文件不可用，释放内存
        paths++;
    }

    // 所有 PATH 路径都尝试失败
    return (*status = CMD_NOT_FOUND, ft_free_split(start_paths), NULL);
}

// 当命令包含路径（如 "./a.out" 或 "/bin/ls"）时直接处理
char *get_direct_path(char **argv, t_status *status)
{
    char *cmd_path;

    cmd_path = ft_strdup(argv[0]); // 复制命令字符串
    if (!cmd_path)
        return ((*status = CMD_NOT_FOUND, NULL));

    if (access(cmd_path, F_OK) == 0) // 文件存在
    {
        if (access(cmd_path, X_OK) == 0) // 可执行
            *status = CMD_SUCCESS;
        else
            *status = CMD_PE_DENIED;   // 文件不可执行
    }
    else
        *status = CMD_NOT_FOUND;       // 文件不存在

    return (cmd_path);
}

// 初始化 t_cmd 结构体
// 包含命令路径 cmd_path 和命令参数 argv 数组
t_cmd *init_cmd(char *argv, char **envp, t_status *status)
{
    t_cmd *cmd;

    // 分配 t_cmd 结构体内存
    cmd = malloc(sizeof(t_cmd));
    if (!cmd)
        return (*status = CMD_MALLOC_ERROR, NULL);

    // 处理空命令或仅空格的情况
    if (is_empty_or_space(argv) == 1)
        cmd->argv = argv_for_empty_space(argv);
    else
        cmd->argv = get_argv(argv); // 正常拆分命令字符串

    if (!cmd->argv)
        return (free(cmd), *status = CMD_MALLOC_ERROR, NULL);

    // 如果命令含有 '/'，表示命令带路径
    if (ft_strchr(cmd->argv[0], '/'))
        cmd->cmd_path = get_direct_path(cmd->argv, status);

    // 空命令或仅空格的特殊处理
    else if (is_empty_or_space(argv) == 1)
    {
        cmd->cmd_path = ft_strdup(argv); // 直接复制原字符串
        *status = CMD_NOT_FOUND;
        if (!cmd->cmd_path)
            return (*status = CMD_MALLOC_ERROR, cmd);
    }

    // 普通命令，通过 PATH 查找完整路径
    else
        cmd->cmd_path = get_cmd_path(cmd->argv, envp, status);

    return (cmd);
}
