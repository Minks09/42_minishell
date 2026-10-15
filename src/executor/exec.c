/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akolomii <akolomii@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:27:01 by akolomii          #+#    #+#             */
/*   Updated: 2026/10/15 16:27:01 by akolomii         ###   ########.fr       */
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
