#include "pipex.h"

/* ************************************************************************** */
/*                                                                            */
/*                     管道与命令执行错误处理函数 (Bonus)                   */
/* ************************************************************************** */

// -----------------------------
// 关闭子进程中使用的所有管道文件描述符
// elist: 存储管道和执行状态的结构体
// -----------------------------
void close_pipes_child_process(t_exe_list *elist)
{
    // 检查并关闭 pipe_fd[0] 的读写端
	if (elist->pipe_fd[0][0] >= 0)
		close(elist->pipe_fd[0][0]);
	if (elist->pipe_fd[0][1] >= 0)
		close(elist->pipe_fd[0][1]);
    // 检查并关闭 pipe_fd[1] 的读写端
	if (elist->pipe_fd[1][0] >= 0)
		close(elist->pipe_fd[1][0]);
	if (elist->pipe_fd[1][1] >= 0)
		close(elist->pipe_fd[1][1]);
}

// -----------------------------
// 文件描述符无效错误处理
// elist: 管道和执行状态信息
// argv: 出错的文件名或命令，用于 perror 打印
// -----------------------------
void handle_invalid_fd_bonus(t_exe_list *elist, char *argv)
{
	close_pipes_child_process(elist); // 关闭所有管道
	perror(argv);                     // 打印系统错误信息
	exit(1);                          // 退出子进程
}

// -----------------------------
// 命令无效错误处理
// elist: 管道和执行状态信息
// fd: 与命令相关的文件描述符，如果有效需要关闭
// cmd: 当前命令结构体，用于释放内存和打印错误
// status: 命令错误类型（未找到/权限/内存等）
// -----------------------------
void handle_invalid_cmd_bonus(t_exe_list *elist, int fd, t_cmd *cmd,
		t_status status)
{
	close_pipes_child_process(elist); // 关闭所有管道
	if (fd >= 0)
		close(fd);                     // 关闭相关文件描述符
	if (cmd)
		ft_print_error(cmd, status);   // 打印命令错误信息
	ft_free_cmd(cmd);                 // 释放命令结构体内存

	// 根据不同错误类型退出
	if (status == CMD_NOT_FOUND)
		exit(CMD_NOT_FOUND);
	else if (status == CMD_PE_DENIED)
		exit(CMD_PE_DENIED);
	else if (status == CMD_MALLOC_ERROR)
		exit(CMD_MALLOC_ERROR);
}

// -----------------------------
// dup2 重定向错误处理
// elist: 管道和执行状态信息
// fd: 当前使用的文件描述符，如果有效需要关闭
// cmd: 当前命令结构体，用于释放内存
// -----------------------------
void handle_dup2_error_bonus(t_exe_list *elist, int fd, t_cmd *cmd)
{
	close_pipes_child_process(elist); // 关闭所有管道
	if (fd >= 0)
		close(fd);                     // 关闭相关文件描述符
	perror("dup2");                   // 打印 dup2 系统错误
	ft_free_cmd(cmd);                 // 释放命令结构体内存
	exit(1);                          // 退出子进程
}
