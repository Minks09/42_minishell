/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minks0x <minks0x@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by minks0x           #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by minks0x          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*tok_name(t_token *tok)
{
	if (!tok)
		return ("newline");
	return (tok->value);
}

int	syntax_check(t_shell *sh)
{
	t_token	*t;

	t = sh->tokens;
	if (t && t->type == T_PIPE)
		return (syntax_error(sh, "|"));
	while (t)
	{
		if (t->type == T_PIPE && (!t->next || t->next->type == T_PIPE))
			return (syntax_error(sh, tok_name(t->next)));
		if (t->type != T_WORD && t->type != T_PIPE
			&& (!t->next || t->next->type != T_WORD))
			return (syntax_error(sh, tok_name(t->next)));
		t = t->next;
	}
	return (1);
}
