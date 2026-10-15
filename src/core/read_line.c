/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akolomii <akolomii@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:26:28 by akolomii          #+#    #+#             */
/*   Updated: 2026/10/15 16:26:29 by akolomii         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*append_char(char *buf, size_t len, char c)
{
	char	*res;

	res = malloc(len + 2);
	if (!res)
	{
		free(buf);
		return (NULL);
	}
	if (buf)
		ft_memcpy(res, buf, len);
	res[len] = c;
	res[len + 1] = '\0';
	free(buf);
	return (res);
}

char	*read_line_fd(int fd)
{
	char	*buf;
	char	c;
	size_t	len;
	ssize_t	r;

	buf = NULL;
	len = 0;
	r = read(fd, &c, 1);
	while (r > 0 && c != '\n')
	{
		buf = ms_check(append_char(buf, len, c));
		len++;
		r = read(fd, &c, 1);
	}
	if (!buf && r > 0)
		return (ms_check(ft_strdup("")));
	return (buf);
}
