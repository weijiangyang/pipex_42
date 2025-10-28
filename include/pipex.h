#ifndef PIPEX_H
# define PIPEX_H

# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <errno.h>
# include "libft.h"

typedef struct s_cmd
{
	char	*cmd_path;
	char	**argv;
}	t_cmd;

typedef struct s_exe_list
{
	int		i;
	int		pipe_fd[2][2];
	int		cmd_count;
	int		here_doc_mode;
	pid_t	pid;
}	t_exe_list;

typedef enum e_status
{
	CMD_SUCCESS = 0,
	CMD_MALLOC_ERROR = 1,
	CMD_PE_DENIED = 126,
	CMD_NOT_FOUND = 127
}	t_status;

//handle_errors.c
int		handle_pipe_error(void);
int		handle_fork_error(void);
void	handle_dup2_error(t_cmd *cmd, int pipe_fd[2], int fd);
void	handle_invalid_fd(int pipe_fd[2], char *argv);
void	handle_invalid_cmd(t_cmd *cmd, int pipe_fd[2], int fd, t_status status);

//handle_errors_bonus.c
void	handle_invalid_fd_bonus(t_exe_list *elist, char *argv);
void	handle_invalid_cmd_bonus(t_exe_list *elist, int fd, t_cmd *cmd,
			t_status status);
void	handle_dup2_error_bonus(t_exe_list *elist, int fd, t_cmd *cmd);
void	close_pipes_child_process(t_exe_list *elist);

//parse_cmd.c
char	**get_path(char **envp, t_status *status);
char	*get_direct_path(char **argv, t_status *status);
t_cmd	*init_cmd(char *argv, char **envp, t_status *status);
void	ft_print_error(t_cmd *cmd, t_status status);

//parse_cmd_utils.c
char	*get_path_helper(char *argv, char *path);
char	**get_argv(char *argv);

//execute_child.c
void	execute_child1(int pipe_fd[2], char **argv, char **envp);
void	execute_child2(int pipe_fd[2], char **argv, char **envp);

//execute_child_bonus.c
void	execute_first_child(t_exe_list *elist, char **argv, char **envp);
void	execute_last_child(t_exe_list *elist, char **argv, char **envp);
void	execute_middle_child(t_exe_list *elist, char **argv, char **envp);
void	execute_process_loop(t_exe_list *elist, char **argv, char **envp);

//execute_child_bonus_utils.c
void	init_pipe_fd(t_exe_list *elist);
int		open_pipe_fd_bonus(t_exe_list *elist);
void	close_pipe_parent_process(t_exe_list *elist);
int		fork_child_process(t_exe_list *elist);

//execute_child_heredoc_bonus.c
void	execute_child_process_heredoc(int *here_doc_fd, t_exe_list *elist,
			char **argv, char **envp);
void	execute_process_loop_heredoc(t_exe_list *elist,
			char **argv, char **envp);
void	execute_first_child_heredoc(int *here_doc_fd, t_exe_list *elist,
			char **argv, char **envp);
void	execute_last_child_heredoc(int *here_doc_fd, t_exe_list *elist,
			char **argv, char **envp);
void	execute_middle_child_heredoc(int *here_doc_fd, t_exe_list *elist,
			char **argv, char **envp);

//read_heredoc.c
void	close_heredoc_fd(int *here_doc_fd);
void	read_here_doc_helper(char *limiter, int *here_doc_fd);
void	read_here_doc(int *here_doc_fd, char **argv);

//parse_cmd_empty_space.c
int		is_empty_or_space(char *argv);
char	**argv_for_empty_space(char *argv);

//main_bonus.c
void	init_elist(int argc, char **argv, t_exe_list *elist);

//ft_free.c
void	ft_free_elist(t_exe_list *elist);
void	ft_free_split(char **str);
void	ft_free_cmd(t_cmd *cmd);

//waitpid_utils.c
int		get_last_exit_code(int status);
int		handle_plural_waitpid(pid_t pid, int cmd_count);
int		handle_waitpid(pid_t pid_2);

#endif