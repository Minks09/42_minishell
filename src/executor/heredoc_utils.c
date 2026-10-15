/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by nachebbi          #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*hd_readline(t_shell *sh)
{
	if (sh->interactive)
		return (readline("> "));
	return (read_line_fd(STDIN_FILENO));
}

void	hd_eof_warning(char *delim)
{
	ft_putstr_fd("minishell: warning: here-document delimited", 2);
	ft_putstr_fd(" by end-of-file (wanted `", 2);
	ft_putstr_fd(delim, 2);
	ft_putendl_fd("')", 2);
}

void	hd_write_line(t_shell *sh, t_redir *r, char *line, int fd)
{
	char	*out;

	out = line;
	if (!r->quoted)
		out = expand_heredoc_line(sh, line);
	ft_putendl_fd(out, fd);
	if (out != line)
		free(out);
	free(line);
}
