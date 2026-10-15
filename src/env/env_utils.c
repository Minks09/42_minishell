/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akolomii <akolomii@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:26:47 by akolomii          #+#    #+#             */
/*   Updated: 2026/10/15 16:26:48 by akolomii         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	env_size(t_env *env)
{
	int	size;

	size = 0;
	while (env)
	{
		size++;
		env = env->next;
	}
	return (size);
}

char	**env_to_arr(t_env *env)
{
	char	**arr;
	char	*tmp;
	int		i;

	arr = ms_check(malloc(sizeof(char *) * (env_size(env) + 1)));
	i = 0;
	while (env)
	{
		if (env->value)
		{
			tmp = ms_check(ft_strjoin(env->key, "="));
			arr[i] = ms_check(ft_strjoin(tmp, env->value));
			free(tmp);
			i++;
		}
		env = env->next;
	}
	arr[i] = NULL;
	return (arr);
}

static void	update_shlvl(t_shell *sh)
{
	char	*val;
	int		lvl;

	val = env_get(sh->env, "SHLVL");
	lvl = 0;
	if (val)
		lvl = ft_atoi(val);
	lvl++;
	if (lvl < 0)
		lvl = 0;
	if (lvl >= 1000)
	{
		ft_putendl_fd("minishell: warning: shell level too high, "
			"resetting to 1", STDERR_FILENO);
		lvl = 1;
	}
	val = ms_check(ft_itoa(lvl));
	env_set(sh, "SHLVL", val);
	free(val);
}

void	env_init(t_shell *sh, char **envp)
{
	char	*eq;
	char	*key;
	char	buf[PATH_MAX];
	int		i;

	i = 0;
	while (envp && envp[i])
	{
		eq = ft_strchr(envp[i], '=');
		if (eq)
		{
			key = ms_check(ft_substr(envp[i], 0, eq - envp[i]));
			env_set(sh, key, eq + 1);
			free(key);
		}
		i++;
	}
	update_shlvl(sh);
	if (!env_find(sh->env, "PWD") && getcwd(buf, sizeof(buf)))
		env_set(sh, "PWD", buf);
	if (!env_find(sh->env, "OLDPWD"))
		env_set(sh, "OLDPWD", NULL);
}
