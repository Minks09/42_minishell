/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minks0x <minks0x@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by minks0x           #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by minks0x          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	parse_exit_arg(char *s, long *out)
{
	long	n;
	int		sign;

	n = 0;
	sign = 1;
	while (ms_isspace(*s))
		s++;
	if (*s == '-')
		sign = -1;
	if (*s == '-' || *s == '+')
		s++;
	if (!ft_isdigit(*s))
		return (0);
	while (ft_isdigit(*s))
	{
		if ((sign > 0 && n > (LONG_MAX - (*s - '0')) / 10)
			|| (sign < 0 && n < (LONG_MIN + (*s - '0')) / 10))
			return (0);
		n = n * 10 + sign * (*s++ - '0');
	}
	while (ms_isspace(*s))
		s++;
	*out = n;
	return (*s == '\0');
}

int	bi_exit(t_shell *sh, char **argv)
{
	long	n;

	if (sh->interactive && !sh->in_child)
		ft_putendl_fd("exit", STDERR_FILENO);
	if (!argv[1])
		ms_exit(sh, sh->status);
	if (!parse_exit_arg(argv[1], &n))
	{
		ms_error("exit", argv[1], "numeric argument required");
		ms_exit(sh, 2);
	}
	if (argv[2])
	{
		ms_error("exit", "too many arguments", NULL);
		return (1);
	}
	ms_exit(sh, (unsigned char)n);
	return (0);
}
