/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by nachebbi          #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	*ms_check(void *ptr)
{
	if (!ptr)
	{
		ft_putendl_fd("minishell: fatal error: out of memory", 2);
		exit(1);
	}
	return (ptr);
}

void	ms_error(char *s1, char *s2, char *s3)
{
	char	buf[1024];

	ft_strlcpy(buf, "minishell: ", sizeof(buf));
	ft_strlcat(buf, s1, sizeof(buf));
	if (s2)
	{
		ft_strlcat(buf, ": ", sizeof(buf));
		ft_strlcat(buf, s2, sizeof(buf));
	}
	if (s3)
	{
		ft_strlcat(buf, ": ", sizeof(buf));
		ft_strlcat(buf, s3, sizeof(buf));
	}
	ft_strlcat(buf, "\n", sizeof(buf));
	write(STDERR_FILENO, buf, ft_strlen(buf));
}

void	ms_error_id(char *cmd, char *arg)
{
	char	buf[1024];

	ft_strlcpy(buf, "minishell: ", sizeof(buf));
	ft_strlcat(buf, cmd, sizeof(buf));
	ft_strlcat(buf, ": `", sizeof(buf));
	ft_strlcat(buf, arg, sizeof(buf));
	ft_strlcat(buf, "': not a valid identifier\n", sizeof(buf));
	write(STDERR_FILENO, buf, ft_strlen(buf));
}

int	syntax_error(t_shell *sh, char *token)
{
	char	buf[256];

	if (!token)
		ft_putendl_fd("minishell: syntax error: unclosed quote", 2);
	else
	{
		ft_strlcpy(buf, "minishell: syntax error near unexpected token `",
			sizeof(buf));
		ft_strlcat(buf, token, sizeof(buf));
		ft_strlcat(buf, "'\n", sizeof(buf));
		write(STDERR_FILENO, buf, ft_strlen(buf));
	}
	sh->status = 2;
	return (0);
}
