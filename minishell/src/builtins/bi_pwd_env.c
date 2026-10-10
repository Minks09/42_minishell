/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_pwd_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minks0x <minks0x@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by minks0x           #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by minks0x          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	bi_pwd(t_shell *sh)
{
	char	buf[PATH_MAX];
	char	*pwd;

	if (getcwd(buf, sizeof(buf)))
	{
		ft_putendl_fd(buf, STDOUT_FILENO);
		return (0);
	}
	pwd = env_get(sh->env, "PWD");
	if (pwd)
	{
		ft_putendl_fd(pwd, STDOUT_FILENO);
		return (0);
	}
	ms_error("pwd", strerror(errno), NULL);
	return (1);
}

int	bi_env(t_shell *sh, char **argv)
{
	t_env	*cur;

	if (argv[1])
	{
		ms_error("env", argv[1], "No such file or directory");
		return (127);
	}
	cur = sh->env;
	while (cur)
	{
		if (cur->value)
		{
			ft_putstr_fd(cur->key, STDOUT_FILENO);
			ft_putchar_fd('=', STDOUT_FILENO);
			ft_putendl_fd(cur->value, STDOUT_FILENO);
		}
		cur = cur->next;
	}
	return (0);
}

int	bi_unset(t_shell *sh, char **argv)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		if (is_valid_name(argv[i], ft_strlen(argv[i])))
			env_unset(sh, argv[i]);
		i++;
	}
	return (0);
}
