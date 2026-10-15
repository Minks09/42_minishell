/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by nachebbi          #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*remove_quotes(char *s)
{
	char	*res;
	size_t	i;
	size_t	j;
	char	q;

	res = ms_check(malloc(ft_strlen(s) + 1));
	i = 0;
	j = 0;
	q = 0;
	while (s[i])
	{
		if (!q && (s[i] == '\'' || s[i] == '"'))
			q = s[i];
		else if (q && s[i] == q)
			q = 0;
		else
			res[j++] = s[i];
		i++;
	}
	res[j] = '\0';
	return (res);
}

int	has_quotes(char *s)
{
	return (ft_strchr(s, '\'') || ft_strchr(s, '"'));
}

static t_redir	*redir_new(t_type type, char *raw)
{
	t_redir	*r;

	r = ms_check(ft_calloc(1, sizeof(t_redir)));
	r->type = type;
	r->fd = -1;
	r->raw = ms_check(ft_strdup(raw));
	return (r);
}

static void	redir_push(t_cmd *cmd, t_redir *r)
{
	t_redir	*cur;

	if (!cmd->redirs)
	{
		cmd->redirs = r;
		return ;
	}
	cur = cmd->redirs;
	while (cur->next)
		cur = cur->next;
	cur->next = r;
}

void	redir_add(t_shell *sh, t_cmd *cmd, t_type type, char *raw)
{
	t_redir	*r;
	t_list	*words;

	r = redir_new(type, raw);
	if (type == T_HEREDOC)
	{
		r->file = remove_quotes(raw);
		r->quoted = has_quotes(raw);
	}
	else
	{
		words = expand_word(sh, raw);
		if (ft_lstsize(words) == 1)
			r->file = ms_check(ft_strdup(words->content));
		ft_lstclear(&words, free);
	}
	redir_push(cmd, r);
}
