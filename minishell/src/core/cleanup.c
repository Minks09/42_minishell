/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minks0x <minks0x@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by minks0x           #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by minks0x          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	tokens_free(t_token **tokens)
{
	t_token	*next;

	while (*tokens)
	{
		next = (*tokens)->next;
		free((*tokens)->value);
		free(*tokens);
		*tokens = next;
	}
}

static void	redirs_free(t_redir *r)
{
	t_redir	*next;

	while (r)
	{
		next = r->next;
		if (r->fd >= 0)
			close(r->fd);
		free(r->file);
		free(r->raw);
		free(r);
		r = next;
	}
}

void	cmds_free(t_cmd **cmds)
{
	t_cmd	*next;

	while (*cmds)
	{
		next = (*cmds)->next;
		ms_free_arr((*cmds)->argv);
		redirs_free((*cmds)->redirs);
		free(*cmds);
		*cmds = next;
	}
}

void	ms_exit(t_shell *sh, int code)
{
	free(sh->line);
	sh->line = NULL;
	tokens_free(&sh->tokens);
	cmds_free(&sh->cmds);
	env_clear(&sh->env);
	if (sh->saved_in >= 0)
		close(sh->saved_in);
	if (sh->saved_out >= 0)
		close(sh->saved_out);
	rl_clear_history();
	exit(code);
}
