/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akolomii <akolomii@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:27:23 by akolomii          #+#    #+#             */
/*   Updated: 2026/10/15 16:27:24 by akolomii         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	open_redir(t_redir *r)
{
	if (r->type == T_IN)
		return (open(r->file, O_RDONLY));
	if (r->type == T_OUT)
		return (open(r->file, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	if (r->type == T_APPEND)
		return (open(r->file, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (-1);
}

static int	apply_one(t_redir *r)
{
	int	fd;
	int	target;

	if (r->type != T_HEREDOC && !r->file)
	{
		ms_error(r->raw, "ambiguous redirect", NULL);
		return (0);
	}
	fd = r->fd;
	r->fd = -1;
	if (r->type != T_HEREDOC)
		fd = open_redir(r);
	if (fd < 0)
	{
		ms_error(r->file, strerror(errno), NULL);
		return (0);
	}
	target = STDOUT_FILENO;
	if (r->type == T_IN || r->type == T_HEREDOC)
		target = STDIN_FILENO;
	dup2(fd, target);
	close(fd);
	return (1);
}

int	apply_redirs(t_cmd *cmd)
{
	t_redir	*r;

	r = cmd->redirs;
	while (r)
	{
		if (!apply_one(r))
			return (0);
		r = r->next;
	}
	return (1);
}

void	close_heredoc_fds(t_cmd *cmds)
{
	t_redir	*r;

	while (cmds)
	{
		r = cmds->redirs;
		while (r)
		{
			if (r->fd >= 0)
				close(r->fd);
			r->fd = -1;
			r = r->next;
		}
		cmds = cmds->next;
	}
}
