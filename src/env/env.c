/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akolomii <akolomii@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:26:51 by akolomii          #+#    #+#             */
/*   Updated: 2026/10/15 16:26:52 by akolomii         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*env_find(t_env *env, char *key)
{
	while (env && ms_strcmp(env->key, key))
		env = env->next;
	return (env);
}

char	*env_get(t_env *env, char *key)
{
	t_env	*node;

	node = env_find(env, key);
	if (!node)
		return (NULL);
	return (node->value);
}

void	env_set(t_shell *sh, char *key, char *value)
{
	t_env	*node;
	t_env	*cur;
	char	*old;

	node = env_find(sh->env, key);
	if (node)
	{
		old = node->value;
		node->value = NULL;
		if (value)
			node->value = ms_check(ft_strdup(value));
		free(old);
		return ;
	}
	node = ms_check(ft_calloc(1, sizeof(t_env)));
	node->key = ms_check(ft_strdup(key));
	if (value)
		node->value = ms_check(ft_strdup(value));
	if (!sh->env)
		sh->env = node;
	cur = sh->env;
	while (cur != node && cur->next)
		cur = cur->next;
	if (cur != node)
		cur->next = node;
}

void	env_unset(t_shell *sh, char *key)
{
	t_env	*cur;
	t_env	*prev;

	cur = sh->env;
	prev = NULL;
	while (cur && ms_strcmp(cur->key, key))
	{
		prev = cur;
		cur = cur->next;
	}
	if (!cur)
		return ;
	if (prev)
		prev->next = cur->next;
	else
		sh->env = cur->next;
	free(cur->key);
	free(cur->value);
	free(cur);
}

void	env_clear(t_env **env)
{
	t_env	*next;

	while (*env)
	{
		next = (*env)->next;
		free((*env)->key);
		free((*env)->value);
		free(*env);
		*env = next;
	}
}
