/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akolomii <akolomii@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:26:34 by akolomii          #+#    #+#             */
/*   Updated: 2026/10/15 16:26:35 by akolomii         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*read_input(t_shell *sh)
{
	if (sh->interactive)
		return (readline(PROMPT));
	return (read_line_fd(STDIN_FILENO));
}

static void	process_line(t_shell *sh)
{
	if (!lexer(sh, sh->line) || !syntax_check(sh))
	{
		tokens_free(&sh->tokens);
		return ;
	}
	sh->cmds = parser(sh);
	tokens_free(&sh->tokens);
	if (sh->cmds && heredocs_prepare(sh))
		execute(sh);
	cmds_free(&sh->cmds);
}

void	shell_loop(t_shell *sh)
{
	while (1)
	{
		signals_interactive(sh->interactive);
		sh->line = read_input(sh);
		if (signal_take() == SIGINT)
			sh->status = 130;
		if (!sh->line)
			break ;
		if (sh->line[0] && sh->interactive)
			add_history(sh->line);
		process_line(sh);
		free(sh->line);
		sh->line = NULL;
	}
	if (sh->interactive)
		ft_putendl_fd("exit", STDERR_FILENO);
}
