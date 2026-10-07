/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minks0x <minks0x@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by minks0x           #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by minks0x          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	var_name_len(char *s)
{
	size_t	len;

	len = 0;
	while (ft_isalnum(s[len]) || s[len] == '_')
		len++;
	return (len);
}

static size_t	append_status(t_exp *e, size_t i)
{
	char	*num;

	num = ms_check(ft_itoa(e->sh->status));
	exp_append(e, num, ft_strlen(num));
	free(num);
	return (i + 1);
}

static size_t	append_dollar(t_exp *e, size_t i)
{
	exp_append(e, "$", 1);
	return (i);
}

size_t	exp_dollar(t_exp *e, char *s, size_t i, int quoted)
{
	size_t	len;
	char	*name;
	char	*val;

	i++;
	if (s[i] == '?')
		return (append_status(e, i));
	if (ft_isdigit(s[i]))
		return (i + 1);
	if (!quoted && (s[i] == '\'' || s[i] == '"'))
		return (i);
	if (!ft_isalpha(s[i]) && s[i] != '_')
		return (append_dollar(e, i));
	len = var_name_len(s + i);
	name = ms_check(ft_substr(s, i, len));
	val = env_get(e->sh->env, name);
	free(name);
	if (val && quoted)
		exp_append(e, val, ft_strlen(val));
	else if (val)
		exp_split_append(e, val);
	return (i + len);
}
