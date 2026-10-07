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
	return (!ft_strcmp(name, "echo") || !ft_strcmp(name, "cd")
		|| !ft_strcmp(name, "pwd") || !ft_strcmp(name, "export")
		|| !ft_strcmp(name, "unset") || !ft_strcmp(name, "env")
		|| !ft_strcmp(name, "exit"));
}

int	run_builtin(t_shell *sh, char **argv)
{
	if (!ft_strcmp(argv[0], "echo"))
		return (bi_echo(argv));
	if (!ft_strcmp(argv[0], "cd"))
		return (bi_cd(sh, argv));
	if (!ft_strcmp(argv[0], "pwd"))
		return (bi_pwd(sh));
	if (!ft_strcmp(argv[0], "export"))
		return (bi_export(sh, argv));
	if (!ft_strcmp(argv[0], "unset"))
		return (bi_unset(sh, argv));
	if (!ft_strcmp(argv[0], "env"))
		return (bi_env(sh, argv));
	return (bi_exit(sh, argv));
}
