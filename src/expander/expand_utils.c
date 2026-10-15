/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by nachebbi          #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exp_append(t_exp *e, char *s, size_t n)
{
	char	*chunk;
	char	*joined;

	e->has_word = 1;
	if (n == 0)
		return ;
	chunk = ms_check(ft_substr(s, 0, n));
	if (!e->buf)
	{
		e->buf = chunk;
		return ;
	}
	joined = ms_check(ft_strjoin(e->buf, chunk));
	free(e->buf);
	free(chunk);
	e->buf = joined;
}

void	exp_flush(t_exp *e)
{
	char	*word;

	if (!e->has_word)
		return ;
	word = e->buf;
	if (!word)
		word = ms_check(ft_strdup(""));
	ft_lstadd_back(&e->words, ms_check(ft_lstnew(word)));
	e->buf = NULL;
	e->has_word = 0;
}

void	exp_split_append(t_exp *e, char *val)
{
	size_t	len;

	while (*val)
	{
		if (*val == ' ' || *val == '\t' || *val == '\n')
		{
			exp_flush(e);
			val++;
			continue ;
		}
		len = 0;
		while (val[len] && val[len] != ' ' && val[len] != '\t'
			&& val[len] != '\n')
			len++;
		exp_append(e, val, len);
		val += len;
	}
}
