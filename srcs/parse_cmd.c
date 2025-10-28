#include "pipex.h"

char **get_path(char **envp, t_status *status)
{
    char **paths;

    while (*envp && ft_strncmp("PATH=", *envp, 5))
        envp++;
    if (!*envp)
        return (*status = CMD_NOT_FOUND, NULL);
    paths = ft_split(*envp + 5, ':');
    if (!paths)
        return (*status = CMD_MALLOC_ERROR, NULL);
    return (paths);
}

char *get_cmd_path(char **argv, char **envp, t_status *status)
{
    char **paths;
    char *cmd_path;
    char **start_paths;

    paths = get_path(envp, status);
    if (!paths)
        return (NULL);
    start_paths = paths;
    while (*paths)
    {
        cmd_path = get_path_helper(argv[0], *paths);
        if (!cmd_path)
            return (*status = CMD_MALLOC_ERROR, ft_free_split(start_paths), NULL);
        if (access(cmd_path, F_OK) == 0 && access(cmd_path, X_OK) == 0)
            return (*status = CMD_SUCCESS, ft_free_split(start_paths), cmd_path);
        else if (access(cmd_path, F_OK) == 0 && access(cmd_path, X_OK) != 0)
            return (*status = CMD_PE_DENIED, ft_free_split(start_paths), NULL);
        free(cmd_path);
        paths++;
    }
    return (*status = CMD_NOT_FOUND, ft_free_split(start_paths), NULL);
}

char *get_direct_path(char **argv, t_status *status)
{
    char *cmd_path;

    cmd_path = ft_strdup(argv[0]);
    if (!cmd_path)
        return ((*status = CMD_NOT_FOUND, NULL));
    if (access(cmd_path, F_OK) == 0)
    {
        if (access(cmd_path, X_OK) == 0)
            *status = CMD_SUCCESS;
        else
            *status = CMD_PE_DENIED;
    }
    else
        *status = CMD_NOT_FOUND;
    return (cmd_path);
}

t_cmd *init_cmd(char *argv, char **envp, t_status *status)
{
    t_cmd *cmd;

    cmd = malloc(sizeof(t_cmd));
   
    if (!cmd)
        return (*status = CMD_MALLOC_ERROR, NULL);
    if (is_empty_or_space(argv) == 1)
        cmd->argv = argv_for_empty_space(argv);
    else
        cmd->argv = get_argv(argv);
    if (!cmd->argv)
        return (free(cmd), *status = CMD_MALLOC_ERROR, NULL);
    if (ft_strchr(cmd->argv[0], '/'))
        cmd->cmd_path = get_direct_path(cmd->argv, status);
    else if (is_empty_or_space(argv) == 1)
    {
        cmd->cmd_path = ft_strdup(argv);
        *status = CMD_NOT_FOUND;
        if (!cmd->cmd_path)
            return (*status = CMD_MALLOC_ERROR, cmd);
    }
    else
        cmd->cmd_path = get_cmd_path(cmd->argv, envp, status);
    return (cmd);
}