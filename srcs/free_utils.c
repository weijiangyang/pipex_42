#include "pipex.h"

void ft_free_split(char **str)
{
    int i;

    i = 0;
    if (!str)
        return;
    while (str[i])
    {
        free(str[i]);
        i++;
    }
    free(str);
}

void ft_free_cmd(t_cmd *cmd)
{
    if (!cmd)
        return;
    if (cmd->argv)
        ft_free_split(cmd->argv);
    if(cmd->cmd_path)
        free(cmd->cmd_path);
    free(cmd);
}