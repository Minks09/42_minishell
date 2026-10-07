/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minks0x <minks0x@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by minks0x           #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by minks0x          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_dir(char *path)
{
	struct stat	st;

	return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

static void	check_exec(t_shell *sh, char *path, char *name)
{
	struct stat	st;

	if (stat(path, &st) != 0)
	{
		ms_error(name, strerror(errno), NULL);
		free(path);
		ms_exit(sh, 127);
	}
	if (S_ISDIR(st.st_mode))
	{
		ms_error(name, "Is a directory", NULL);
		free(path);
		ms_exit(sh, 126);
	}
	if (access(path, X_OK) != 0)
	{
		ms_error(name, strerror(errno), NULL);
		free(path);
		ms_exit(sh, 126);
	}
}

static char	*search_path(char *name, char *path_env)
{
	char	**dirs;
	char	*dir;
	char	*full;
	int		i;

	dirs = ms_check(ft_split(path_env, ':'));
	full = NULL;
	i = 0;
	while (dirs[i] && !full)
	{
		dir = ms_check(ft_strjoin(dirs[i], "/"));
		full = ms_check(ft_strjoin(dir, name));
		free(dir);
		if (access(full, F_OK) != 0 || is_dir(full))
		{
			free(full);
			full = NULL;
		}
		i++;
	}
	ft_free_arr(dirs);
	return (full);
}

static char	*find_command(t_shell *sh, char *name)
{
	char	*path_env;
	char	*full;

	path_env = env_get(sh->env, "PATH");
	full = NULL;
	if (name[0] && (ft_strchr(name, '/') || !path_env || !path_env[0]))
		full = ms_check(ft_strdup(name));
	else if (name[0])
		full = search_path(name, path_env);
	if (!full)
	{
		ms_error(name, "command not found", NULL);
		ms_exit(sh, 127);
	}
	check_exec(sh, full, name);
	return (full);
}

void	exec_external(t_shell *sh, char **argv)
{
	char	*path;
	char	**envp;

	path = find_command(sh, argv[0]);
	envp = env_to_arr(sh->env);
	execve(path, argv, envp);
	ms_error(argv[0], strerror(errno), NULL);
	free(path);
	ft_free_arr(envp);
	ms_exit(sh, 126);
}
