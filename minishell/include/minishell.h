/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minks0x <minks0x@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 23:00:00 by minks0x           #+#    #+#             */
/*   Updated: 2026/09/28 23:00:00 by minks0x          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <signal.h>
# include <errno.h>
# include <string.h>
# include <limits.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft.h"

# ifndef PATH_MAX
#  define PATH_MAX 4096
# endif

# define PROMPT "minishell$ "
# define HD_PREFIX "/tmp/.minishell_heredoc_"

extern volatile sig_atomic_t	g_signal;

typedef enum e_type
{
	T_WORD,
	T_PIPE,
	T_IN,
	T_OUT,
	T_APPEND,
	T_HEREDOC
}	t_type;

typedef struct s_token
{
	t_type			type;
	char			*value;
	struct s_token	*next;
}	t_token;

typedef struct s_redir
{
	t_type			type;
	char			*file;
	char			*raw;
	int				quoted;
	int				fd;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	char			**argv;
	t_redir			*redirs;
	pid_t			pid;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_shell
{
	t_env	*env;
	t_token	*tokens;
	t_cmd	*cmds;
	char	*line;
	int		status;
	int		interactive;
	int		in_child;
	int		hd_count;
	int		saved_in;
	int		saved_out;
}	t_shell;

typedef struct s_exp
{
	t_list	*words;
	char	*buf;
	int		has_word;
	t_shell	*sh;
}	t_exp;

/* core */
void	shell_loop(t_shell *sh);
char	*read_line_fd(int fd);
void	*ms_check(void *ptr);
void	ms_error(char *s1, char *s2, char *s3);
void	ms_error_id(char *cmd, char *arg);
int		syntax_error(t_shell *sh, char *token);
void	tokens_free(t_token **tokens);
void	cmds_free(t_cmd **cmds);
void	ms_exit(t_shell *sh, int code);

/* lexer */
int		lexer(t_shell *sh, char *s);
int		syntax_check(t_shell *sh);

/* expander */
t_list	*expand_word(t_shell *sh, char *s);
char	*expand_heredoc_line(t_shell *sh, char *line);
size_t	exp_dollar(t_exp *e, char *s, size_t i, int quoted);
void	exp_append(t_exp *e, char *s, size_t n);
void	exp_flush(t_exp *e);
void	exp_split_append(t_exp *e, char *val);

/* parser */
t_cmd	*parser(t_shell *sh);
void	redir_add(t_shell *sh, t_cmd *cmd, t_type type, char *raw);
char	*remove_quotes(char *s);
int		has_quotes(char *s);

/* executor */
void	execute(t_shell *sh);
void	exec_pipeline(t_shell *sh);
int		apply_redirs(t_cmd *cmd);
void	close_heredoc_fds(t_cmd *cmds);
void	exec_external(t_shell *sh, char **argv);
int		heredocs_prepare(t_shell *sh);
char	*hd_readline(t_shell *sh);
void	hd_eof_warning(char *delim);
void	hd_write_line(t_shell *sh, t_redir *r, char *line, int fd);

/* builtins */
int		is_builtin(char *name);
int		run_builtin(t_shell *sh, char **argv);
int		bi_echo(char **argv);
int		bi_cd(t_shell *sh, char **argv);
int		bi_pwd(t_shell *sh);
int		bi_env(t_shell *sh, char **argv);
int		bi_unset(t_shell *sh, char **argv);
int		bi_export(t_shell *sh, char **argv);
void	export_print(t_shell *sh);
int		is_valid_name(char *s, size_t len);
int		bi_exit(t_shell *sh, char **argv);

/* env */
void	env_init(t_shell *sh, char **envp);
t_env	*env_find(t_env *env, char *key);
char	*env_get(t_env *env, char *key);
void	env_set(t_shell *sh, char *key, char *value);
void	env_unset(t_shell *sh, char *key);
void	env_clear(t_env **env);
int		env_size(t_env *env);
char	**env_to_arr(t_env *env);

/* utils */
int		ms_strcmp(const char *s1, const char *s2);
int		ms_isspace(int c);
void	ms_free_arr(char **arr);

/* signals */
void	signals_interactive(int interactive);
void	signals_ignore(void);
void	signals_default(void);

#endif
