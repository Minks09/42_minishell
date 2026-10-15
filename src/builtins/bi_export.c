/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/17 23:03:16 by nachebbi          #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_valid_name(char *s, size_t len)
{
	size_t	i;

	if (len == 0 || (!ft_isalpha(s[0]) && s[0] != '_'))
		return (0);
	i = 0;
	while (i < len)
	{
		if (!ft_isalnum(s[i]) && s[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static void	export_assign(t_shell *sh, char *key, char *value, int append)
{
	char	*old;
	char	*joined;

	if (!append)
	{
		env_set(sh, key, value);
		return ;
	}
	old = env_get(sh->env, key);
	if (!old)
		old = "";
	joined = ms_check(ft_strjoin(old, value));
	env_set(sh, key, joined);
	free(joined);
}

static int	export_one(t_shell *sh, char *arg)
{
	size_t	len;
	char	*key;
	int		append;

	len = 0;
	while (arg[len] && arg[len] != '=' && arg[len] != '+')
		len++;
	append = (arg[len] == '+' && arg[len + 1] == '=');
	if (!is_valid_name(arg, len) || (arg[len] == '+' && !append))
	{
		ms_error_id("export", arg);
		return (0);
	}
	key = ms_check(ft_substr(arg, 0, len));
	if (!arg[len] && !env_find(sh->env, key))
		env_set(sh, key, NULL);
	else if (arg[len])
		export_assign(sh, key, arg + len + 1 + append, append);
	free(key);
	return (1);
}

int	bi_export(t_shell *sh, char **argv)
{
	int	i;
	int	ret;

	if (!argv[1])
	{
		export_print(sh);
		return (0);
	}
	ret = 0;
	i = 1;
	while (argv[i])
	{
		if (!export_one(sh, argv[i]))
			ret = 1;
		i++;
	}
	return (ret);
}
