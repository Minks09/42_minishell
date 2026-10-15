/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by nachebbi          #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	exp_squote(t_exp *e, char *s, size_t i)
{
	size_t	len;

	i++;
	len = 0;
	while (s[i + len] && s[i + len] != '\'')
		len++;
	exp_append(e, s + i, len);
	if (!s[i + len])
		return (i + len);
	return (i + len + 1);
}

static size_t	exp_dquote(t_exp *e, char *s, size_t i)
{
	size_t	len;

	i++;
	exp_append(e, "", 0);
	while (s[i] && s[i] != '"')
	{
		if (s[i] == '$')
			i = exp_dollar(e, s, i, 1);
		else
		{
			len = 0;
			while (s[i + len] && s[i + len] != '"' && s[i + len] != '$')
				len++;
			exp_append(e, s + i, len);
			i += len;
		}
	}
	if (!s[i])
		return (i);
	return (i + 1);
}

static size_t	exp_plain(t_exp *e, char *s, size_t i)
{
	size_t	len;

	len = 0;
	while (s[i + len] && s[i + len] != '\'' && s[i + len] != '"'
		&& s[i + len] != '$')
		len++;
	exp_append(e, s + i, len);
	return (i + len);
}

t_list	*expand_word(t_shell *sh, char *s)
{
	t_exp	e;
	size_t	i;

	e.words = NULL;
	e.buf = NULL;
	e.has_word = 0;
	e.sh = sh;
	i = 0;
	while (s[i])
	{
		if (s[i] == '\'')
			i = exp_squote(&e, s, i);
		else if (s[i] == '"')
			i = exp_dquote(&e, s, i);
		else if (s[i] == '$')
			i = exp_dollar(&e, s, i, 0);
		else
			i = exp_plain(&e, s, i);
	}
	exp_flush(&e);
	return (e.words);
}

char	*expand_heredoc_line(t_shell *sh, char *line)
{
	t_exp	e;
	size_t	i;
	size_t	len;

	e.words = NULL;
	e.buf = NULL;
	e.has_word = 0;
	e.sh = sh;
	i = 0;
	while (line[i])
	{
		if (line[i] == '$')
			i = exp_dollar(&e, line, i, 1);
		else
		{
			len = 0;
			while (line[i + len] && line[i + len] != '$')
				len++;
			exp_append(&e, line + i, len);
			i += len;
		}
	}
	if (!e.buf)
		return (ms_check(ft_strdup("")));
	return (e.buf);
}
