/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by nachebbi          #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_meta(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

static int	op_len(char *s, t_type *type)
{
	if (s[0] == '|')
		*type = T_PIPE;
	else if (s[0] == '<' && s[1] == '<')
		*type = T_HEREDOC;
	else if (s[0] == '>' && s[1] == '>')
		*type = T_APPEND;
	else if (s[0] == '<')
		*type = T_IN;
	else
		*type = T_OUT;
	if (*type == T_HEREDOC || *type == T_APPEND)
		return (2);
	return (1);
}

static int	word_len(char *s, int *len)
{
	int		i;
	char	q;

	i = 0;
	while (s[i] && !ms_isspace(s[i]) && !is_meta(s[i]))
	{
		if (s[i] == '\'' || s[i] == '"')
		{
			q = s[i++];
			while (s[i] && s[i] != q)
				i++;
			if (!s[i])
				return (0);
		}
		i++;
	}
	*len = i;
	return (1);
}

static void	token_add(t_shell *sh, t_type type, char *value)
{
	t_token	*tok;
	t_token	*cur;

	tok = ms_check(malloc(sizeof(t_token)));
	tok->type = type;
	tok->value = value;
	tok->next = NULL;
	if (!sh->tokens)
	{
		sh->tokens = tok;
		return ;
	}
	cur = sh->tokens;
	while (cur->next)
		cur = cur->next;
	cur->next = tok;
}

int	lexer(t_shell *sh, char *s)
{
	size_t	i;
	int		len;
	t_type	type;

	i = 0;
	while (s[i])
	{
		if (ms_isspace(s[i]))
		{
			i++;
			continue ;
		}
		type = T_WORD;
		if (is_meta(s[i]))
			len = op_len(s + i, &type);
		else if (!word_len(s + i, &len))
			return (syntax_error(sh, NULL));
		token_add(sh, type, ms_check(ft_substr(s, i, len)));
		i += len;
	}
	return (1);
}
