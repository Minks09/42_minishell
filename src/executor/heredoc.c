/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by nachebbi          #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	hd_open_tmp(t_shell *sh, char **path)
{
	char	*num;
	int		fd;
	int		tries;

	tries = 0;
	while (tries++ < 10000)
	{
		num = ms_check(ft_itoa(sh->hd_count++));
		*path = ms_check(ft_strjoin(HD_PREFIX, num));
		free(num);
		fd = open(*path, O_WRONLY | O_CREAT | O_EXCL, 0600);
		if (fd >= 0)
			return (fd);
		free(*path);
		*path = NULL;
		if (errno != EEXIST)
			break ;
	}
	ms_error("heredoc", strerror(errno), NULL);
	return (-1);
}

static void	hd_child(t_shell *sh, t_redir *r, int fd, char *path)
{
	char	*line;

	free(path);
	sh->in_child = 1;
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	while (1)
	{
		line = hd_readline(sh);
		if (!line)
		{
			hd_eof_warning(r->file);
			break ;
		}
		if (!ms_strcmp(line, r->file))
		{
			free(line);
			break ;
		}
		hd_write_line(sh, r, line, fd);
	}
	close(fd);
	ms_exit(sh, 0);
}

static int	hd_fail(t_shell *sh, pid_t pid, int st, int fd)
{
	sh->status = 1;
	if (pid < 0 || fd < 0)
		ms_error("heredoc", "failed to create here-document", NULL);
	if (pid > 0 && WIFSIGNALED(st))
		sh->status = 128 + WTERMSIG(st);
	if (pid > 0 && WIFSIGNALED(st) && WTERMSIG(st) == SIGINT)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}

static int	hd_read_one(t_shell *sh, t_redir *r)
{
	char	*path;
	int		fd;
	int		st;
	pid_t	pid;

	fd = hd_open_tmp(sh, &path);
	if (fd < 0)
		return (0);
	pid = fork();
	if (pid == 0)
		hd_child(sh, r, fd, path);
	close(fd);
	st = 0;
	if (pid > 0)
		waitpid(pid, &st, 0);
	r->fd = open(path, O_RDONLY);
	unlink(path);
	free(path);
	if (pid > 0 && !WIFSIGNALED(st) && r->fd >= 0)
		return (1);
	return (hd_fail(sh, pid, st, r->fd));
}

int	heredocs_prepare(t_shell *sh)
{
	t_cmd	*cmd;
	t_redir	*r;

	signals_ignore();
	cmd = sh->cmds;
	while (cmd)
	{
		r = cmd->redirs;
		while (r)
		{
			if (r->type == T_HEREDOC && !hd_read_one(sh, r))
				return (0);
			r = r->next;
		}
		cmd = cmd->next;
	}
	return (1);
}
