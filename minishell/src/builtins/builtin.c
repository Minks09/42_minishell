/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minks0x <minks0x@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by minks0x           #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by minks0x          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_builtin(char *name)
{
	return (!ms_strcmp(name, "echo") || !ms_strcmp(name, "cd")
		|| !ms_strcmp(name, "pwd") || !ms_strcmp(name, "export")
		|| !ms_strcmp(name, "unset") || !ms_strcmp(name, "env")
		|| !ms_strcmp(name, "exit"));
}

int	run_builtin(t_shell *sh, char **argv)
{
	if (!ms_strcmp(argv[0], "echo"))
		return (bi_echo(argv));
	if (!ms_strcmp(argv[0], "cd"))
		return (bi_cd(sh, argv));
	if (!ms_strcmp(argv[0], "pwd"))
		return (bi_pwd(sh));
	if (!ms_strcmp(argv[0], "export"))
		return (bi_export(sh, argv));
	if (!ms_strcmp(argv[0], "unset"))
		return (bi_unset(sh, argv));
	if (!ms_strcmp(argv[0], "env"))
		return (bi_env(sh, argv));
	return (bi_exit(sh, argv));
}
