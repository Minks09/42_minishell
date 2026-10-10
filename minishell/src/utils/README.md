# `utils/` — Helpers that are not in libft

| File | Functions |
|---|---|
| [`utils.c`](utils.c) | `ms_strcmp`, `ms_isspace`, `ms_free_arr` |

The project's [libft](../../libft) provides most string / memory / list
functions. These three are needed by minishell but are not part of the libft
subject, so they live here with an `ms_` prefix (no name clash if libft gains
an `ft_strcmp` one day).

| Function | Does | Used by |
|---|---|---|
| `int ms_strcmp(s1, s2)` | Compares two strings fully (`<0`, `0`, `>0`), like `strcmp`. | builtin names, `env_find`, heredoc delimiter, `export` sort |
| `int ms_isspace(c)` | `' '`, `\t`, `\n`, `\v`, `\f`, `\r` → true. | lexer (token separation), `exit` argument parsing |
| `void ms_free_arr(arr)` | Frees each string of a `NULL`-terminated `char **`, then the array. Accepts `NULL`. | `cmds_free` (argv), `search_path` (PATH dirs), `exec_external` (envp) |

```
ms_free_arr(arr)

arr ──► [ ptr0 | ptr1 | ptr2 | NULL ]
           │      │      │
           ▼      ▼      ▼
         "ls"   "-l"   "/tmp"

  1. free("ls"), free("-l"), free("/tmp")
  2. free(arr)
```

## libft functions used by minishell

| Group | Functions |
|---|---|
| strings | `ft_strlen`, `ft_strdup`, `ft_substr`, `ft_strjoin`, `ft_strchr`, `ft_strlcpy`, `ft_strlcat`, `ft_split` |
| chars | `ft_isalpha`, `ft_isdigit`, `ft_isalnum` |
| numbers | `ft_atoi` (`SHLVL`), `ft_itoa` (`$?`, `SHLVL`, heredoc file names) |
| memory | `ft_calloc`, `ft_bzero`, `ft_memcpy` |
| output | `ft_putchar_fd`, `ft_putstr_fd`, `ft_putendl_fd` |
| lists (bonus) | `ft_lstnew`, `ft_lstadd_back`, `ft_lstsize`, `ft_lstclear` (expander word list) |

The list functions are part of the libft **bonus**, which is why the
minishell `Makefile` builds libft with `make -C libft bonus`.
