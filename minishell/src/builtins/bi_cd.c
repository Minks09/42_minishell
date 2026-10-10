/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minks0x <minks0x@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by minks0x           #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by minks0x          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*cd_target(t_shell *sh, char **argv)
{
	char	*target;

	if (!argv[1])
	{
		target = env_get(sh->env, "HOME");
		if (!target)
			ms_error("cd", "HOME not set", NULL);
		return (target);
	}
	if (!ms_strcmp(argv[1], "-"))
	{
		target = env_get(sh->env, "OLDPWD");
		if (!target)
			ms_error("cd", "OLDPWD not set", NULL);
		return (target);
	}
	return (argv[1]);
}

static void	update_pwd(t_shell *sh, char *oldcwd)
{
	char	buf[PATH_MAX];

	if (oldcwd[0])
		env_set(sh, "OLDPWD", oldcwd);
	else if (env_get(sh->env, "PWD"))
		env_set(sh, "OLDPWD", env_get(sh->env, "PWD"));
	if (getcwd(buf, sizeof(buf)))
		env_set(sh, "PWD", buf);
}

int	bi_cd(t_shell *sh, char **argv)
{
	char	*target;
	char	cwd[PATH_MAX];

	if (argv[1] && argv[2])
	{
		ms_error("cd", "too many arguments", NULL);
		return (1);
	}
	target = cd_target(sh, argv);
	if (!target)
		return (1);
	if (!target[0])
		return (0);
	if (!getcwd(cwd, sizeof(cwd)))
		ft_strlcpy(cwd, "", sizeof(cwd));
	if (chdir(target) != 0)
	{
		ms_error("cd", target, strerror(errno));
		return (1);
	}
	update_pwd(sh, cwd);
	if (argv[1] && !ms_strcmp(argv[1], "-"))
		bi_pwd(sh);
	return (0);
}
