/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minks0x <minks0x@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by minks0x           #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by minks0x          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	run_builtin_parent(t_shell *sh, t_cmd *cmd)
{
	int	ret;

	sh->saved_in = dup(STDIN_FILENO);
	sh->saved_out = dup(STDOUT_FILENO);
	ret = 1;
	if (apply_redirs(cmd))
		ret = run_builtin(sh, cmd->argv);
	dup2(sh->saved_in, STDIN_FILENO);
	dup2(sh->saved_out, STDOUT_FILENO);
	close(sh->saved_in);
	close(sh->saved_out);
	sh->saved_in = -1;
	sh->saved_out = -1;
	return (ret);
}

void	execute(t_shell *sh)
{
	t_cmd	*cmd;

	cmd = sh->cmds;
	if (!cmd->next && cmd->argv[0] && is_builtin(cmd->argv[0]))
		sh->status = run_builtin_parent(sh, cmd);
	else
		exec_pipeline(sh);
}
