/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 23:10:25 by nachebbi          #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**list_to_arr(t_list **lst)
{
	char	**arr;
	t_list	*cur;
	int		i;

	arr = ms_check(malloc(sizeof(char *) * (ft_lstsize(*lst) + 1)));
	cur = *lst;
	i = 0;
	while (cur)
	{
		arr[i++] = cur->content;
		cur->content = NULL;
		cur = cur->next;
	}
	arr[i] = NULL;
	ft_lstclear(lst, free);
	return (arr);
}

static t_cmd	*parse_cmd(t_shell *sh, t_token **tok)
{
	t_cmd	*cmd;
	t_list	*args;
	t_token	*t;

	cmd = ms_check(ft_calloc(1, sizeof(t_cmd)));
	cmd->pid = -1;
	args = NULL;
	t = *tok;
	while (t && t->type != T_PIPE)
	{
		if (t->type == T_WORD)
			ft_lstadd_back(&args, expand_word(sh, t->value));
		else
		{
			redir_add(sh, cmd, t->type, t->next->value);
			t = t->next;
		}
		t = t->next;
	}
	cmd->argv = list_to_arr(&args);
	*tok = t;
	return (cmd);
}

t_cmd	*parser(t_shell *sh)
{
	t_token	*t;
	t_cmd	*head;
	t_cmd	*last;
	t_cmd	*cmd;

	t = sh->tokens;
	head = NULL;
	last = NULL;
	while (t)
	{
		cmd = parse_cmd(sh, &t);
		if (!head)
			head = cmd;
		else
			last->next = cmd;
		last = cmd;
		if (t)
			t = t->next;
	}
	return (head);
}
