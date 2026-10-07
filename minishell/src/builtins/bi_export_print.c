/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_export_print.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minks0x <minks0x@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by minks0x           #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by minks0x          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	sort_env(t_env **arr, int n)
{
	int		i;
	int		j;
	t_env	*tmp;

	i = 0;
	while (i < n - 1)
	{
		j = 0;
		while (j < n - i - 1)
		{
			if (ft_strcmp(arr[j]->key, arr[j + 1]->key) > 0)
			{
				tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
}

static void	print_escaped(char *s)
{
	while (*s)
	{
		if (*s == '"' || *s == '\\' || *s == '$' || *s == '`')
			write(STDOUT_FILENO, "\\", 1);
		write(STDOUT_FILENO, s, 1);
		s++;
	}
}

static void	print_entry(t_env *e)
{
	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	ft_putstr_fd(e->key, STDOUT_FILENO);
	if (e->value)
	{
		ft_putstr_fd("=\"", STDOUT_FILENO);
		print_escaped(e->value);
		ft_putstr_fd("\"", STDOUT_FILENO);
	}
	ft_putstr_fd("\n", STDOUT_FILENO);
}

void	export_print(t_shell *sh)
{
	t_env	**arr;
	t_env	*cur;
	int		n;
	int		i;

	arr = ms_check(malloc(sizeof(t_env *) * (env_size(sh->env) + 1)));
	cur = sh->env;
	n = 0;
	while (cur)
	{
		arr[n++] = cur;
		cur = cur->next;
	}
	sort_env(arr, n);
	i = 0;
	while (i < n)
	{
		if (ft_strcmp(arr[i]->key, "_"))
			print_entry(arr[i]);
		i++;
	}
	free(arr);
}
