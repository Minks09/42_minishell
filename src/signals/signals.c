/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akolomii <akolomii@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:27:42 by akolomii          #+#    #+#             */
/*   Updated: 2026/10/15 16:27:42 by akolomii         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static volatile sig_atomic_t	g_signal = 0;

static void	on_sigint_prompt(int sig)
{
	g_signal = sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	signals_interactive(int interactive)
{
	if (interactive)
		signal(SIGINT, on_sigint_prompt);
	else
		signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
}

void	signals_ignore(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

int	signal_take(void)
{
	int	sig;

	sig = g_signal;
	g_signal = 0;
	return (sig);
}

void	signals_default(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
