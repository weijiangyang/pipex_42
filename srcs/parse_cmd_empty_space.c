#include "pipex.h"

int is_empty_or_space(char *argv)
{
    int i;

    i = 0;
    if (!argv || !*argv)
        return (1);
    while (argv[i])
    {
        if (!(argv[i] == ' ' || (argv[i] >= 9 && argv[i] <= 13)))
            return (0);
        i++;
    }
    return (1);
}

char **argv_for_empty_space(char *argv)
{
    char **cmd_argv;

    cmd_argv = malloc (sizeof(char *) * 2);
    if (!cmd_argv)
        return (NULL);
    cmd_argv[0] = ft_strdup(argv);
    if (!cmd_argv[0])
        return (free(cmd_argv), NULL);
    cmd_argv[1] = NULL;
    return (cmd_argv);
}