#include "pipex.h"

char    *get_path_helper(char *argv, char *path)
{
    char *tmp;
    char *cmd_path;

    tmp = ft_strjoin(path, "/");
    if (!tmp)
        return (NULL);
    cmd_path = ft_strjoin(tmp, argv);
    if (!cmd_path)
        return (NULL);
    free (tmp);
    return (cmd_path);
}

char **get_argv(char *argv)
{
    char **argv_cmd;

    argv_cmd = ft_split(argv, ' ');
    if (!argv_cmd)
        return (NULL);
    return (argv_cmd);
}