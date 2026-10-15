/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akolomii <akolomii@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:27:19 by akolomii          #+#    #+#             */
/*   Updated: 2026/10/15 16:27:19 by akolomii         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	child_process(t_shell *sh, t_cmd *cmd, int prev, int *fds)
{
	sh->in_child = 1;
	signals_default();
	if (prev >= 0)
	{
		dup2(prev, STDIN_FILENO);
		close(prev);
	}
	if (fds[1] >= 0)
	{
		dup2(fds[1], STDOUT_FILENO);
		close(fds[1]);
		close(fds[0]);
	}
	if (!apply_redirs(cmd))
		ms_exit(sh, 1);
	close_heredoc_fds(sh->cmds);
	if (!cmd->argv[0])
		ms_exit(sh, 0);
	if (is_builtin(cmd->argv[0]))
		ms_exit(sh, run_builtin(sh, cmd->argv));
	exec_external(sh, cmd->argv);
}

static int	spawn(t_shell *sh, t_cmd *cmd, int prev)
{
	int	fds[2];

	fds[0] = -1;
	fds[1] = -1;
	if (cmd->next && pipe(fds) < 0)
	{
		ms_error("pipe", strerror(errno), NULL);
		return (-2);
	}
	cmd->pid = fork();
	if (cmd->pid < 0)
		ms_error("fork", strerror(errno), NULL);
	if (cmd->pid == 0)
		child_process(sh, cmd, prev, fds);
	if (fds[1] >= 0)
		close(fds[1]);
	if (cmd->pid < 0 && fds[0] >= 0)
		close(fds[0]);
	if (cmd->pid < 0)
		return (-2);
	return (fds[0]);
}

static void	wait_all(t_shell *sh)
{
	t_cmd	*cmd;
	int		st;
	int		sig;

	cmd = sh->cmds;
	sig = 0;
	st = 0;
	while (cmd)
	{
		if (cmd->pid > 0 && waitpid(cmd->pid, &st, 0) > 0)
		{
			sig = 0;
			if (WIFSIGNALED(st))
				sig = WTERMSIG(st);
			sh->status = WEXITSTATUS(st);
			if (sig)
				sh->status = 128 + sig;
		}
		cmd = cmd->next;
	}
	if (sig == SIGINT)
		ft_putchar_fd('\n', STDOUT_FILENO);
	else if (sig == SIGQUIT)
		ft_putendl_fd("Quit (core dumped)", STDERR_FILENO);
}

void	exec_pipeline(t_shell *sh)
{
	t_cmd	*cmd;
	int		prev;
	int		next;

	signals_ignore();
	prev = -1;
	next = 0;
	cmd = sh->cmds;
	while (cmd)
	{
		next = spawn(sh, cmd, prev);
		if (prev >= 0)
			close(prev);
		if (next == -2)
			break ;
		prev = next;
		cmd = cmd->next;
	}
	wait_all(sh);
	if (next == -2)
		sh->status = 1;
}
