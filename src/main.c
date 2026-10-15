/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 16:04:42 by nachebbi          #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char **argv, char **envp)
{
	t_shell	sh;

	(void)argv;
	if (argc != 1)
	{
		ft_putendl_fd("minishell: this shell takes no arguments", 2);
		return (1);
	}
	ft_bzero(&sh, sizeof(t_shell));
	sh.saved_in = -1;
	sh.saved_out = -1;
	sh.interactive = isatty(STDIN_FILENO) && isatty(STDERR_FILENO);
	env_init(&sh, envp);
	shell_loop(&sh);
	ms_exit(&sh, sh.status);
	return (0);
}
