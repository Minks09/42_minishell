/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_pwd_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akolomii <akolomii@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 16:26:47 by akolomii          #+#    #+#             */
/*   Updated: 2026/10/15 16:26:48 by akolomii         ###   ########.fr       */
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
