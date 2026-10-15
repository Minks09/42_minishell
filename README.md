# minishell

A small Bash-like shell written in C for the 42 cursus.

## Build

Requires GNU readline headers (`sudo apt install libreadline-dev` on Debian/Ubuntu).

```bash
make            # build ./minishell
make clean      # remove objects
make fclean     # remove objects + binary
make re         # rebuild
```

On macOS, install GNU readline with `brew install readline`; the Makefile
picks it up automatically (the system readline is libedit and lacks
`rl_clear_history` / `rl_replace_line`).

## Features

- Prompt with history (readline)
- Command lookup via `PATH`, or relative/absolute paths (127 not found, 126 not executable / directory)
- Quotes: `'...'` blocks all interpretation, `"..."` blocks everything except `$`
- Redirections: `<`, `>`, `>>`, `<<` (heredoc; quoted delimiter disables expansion)
- Pipes: `cmd1 | cmd2 | ... | cmdN`
- Expansion of `$VAR` and `$?`, with word splitting on unquoted expansions
- Signals: `ctrl-C` new prompt (status 130), `ctrl-D` exits, `ctrl-\` ignored at the prompt;
  children get default behaviour (`Quit (core dumped)`, status 131)
- `SHLVL` handling, env inherited from parent
- Unclosed quotes, `\` and `;` are not interpreted (as required by the subject)

### Builtins

| Builtin  | Notes                                                       |
|----------|-------------------------------------------------------------|
| `echo`   | `-n` (also `-nnn`, repeated flags)                          |
| `cd`     | relative/absolute path, no arg → `$HOME`, `-` → `$OLDPWD`   |
| `pwd`    | no options                                                  |
| `export` | `KEY=val`, `KEY`, `KEY+=val`; sorted `declare -x` listing   |
| `unset`  | validates identifiers                                       |
| `env`    | prints variables that have a value                          |
| `exit`   | `exit [n]`, numeric check, `too many arguments`             |

## Architecture

Detailed documentation with diagrams: start at [`src/README.md`](src/README.md),
each `src/` sub-folder has its own `README.md`.

```
line ──► lexer ──► syntax check ──► parser (+ expansion) ──► heredocs ──► executor
         tokens     `|` / redir      t_cmd list: argv +        temp files    builtin in parent
                    errors → $?=2    t_redir list              (forked)      or fork/pipe/execve
```

| Directory        | Role                                                         |
|------------------|--------------------------------------------------------------|
| `src/core`       | main loop, non-tty line reader, errors, cleanup              |
| `src/lexer`      | tokenizer and syntax validation                              |
| `src/expander`   | `$VAR` / `$?` expansion, quote removal, word splitting       |
| `src/parser`     | tokens → command list with redirections                      |
| `src/executor`   | pipelines, redirections, PATH resolution, heredocs           |
| `src/builtins`   | the seven builtins                                           |
| `src/env`        | environment linked list                                      |
| `src/signals`    | signal handlers (single global: `g_signal`)                  |
| `libft`          | own minimal libft                                            |

## Testing for leaks

```bash
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
         --trace-children=yes --suppressions=readline.supp ./minishell
```

(readline's own allocations are not required to be freed by the subject.)
