*This project has been created as part of the 42 curriculum by nachebbi, akolomii.*

# minishell

## Description

minishell is a small Bash-like shell written in C. The goal of the project is to
understand how a Unix shell works from the inside: reading and parsing a command
line, managing processes with `fork` / `execve` / `waitpid`, connecting them with
pipes, redirecting file descriptors, handling signals and maintaining an
environment.

The shell reads a line, splits it into tokens, checks the syntax, expands
variables and quotes, builds a list of commands, and then executes them, with
Bash as the reference for behaviour and error codes.

### Features

- Prompt with working history (readline)
- Command lookup via `PATH`, or relative/absolute paths (127 not found, 126 not executable / directory)
- Quotes: `'...'` blocks all interpretation, `"..."` blocks everything except `$`
- Redirections: `<`, `>`, `>>`, `<<` (heredoc; quoted delimiter disables expansion)
- Pipes: `cmd1 | cmd2 | ... | cmdN`
- Expansion of `$VAR` and `$?`, with word splitting on unquoted expansions
- Signals: `ctrl-C` new prompt (status 130), `ctrl-D` exits, `ctrl-\` ignored at the prompt;
  children get default behaviour (`Quit (core dumped)`, status 131)
- `SHLVL`, `PWD` and `OLDPWD` handling, environment inherited from the parent
- Unclosed quotes, `\` and `;` are not interpreted (as required by the subject)

### Builtins

| Builtin  | Notes                                                       |
|----------|-------------------------------------------------------------|
| `echo`   | `-n` (also `-nnn`, repeated flags)                          |
| `cd`     | relative/absolute path, no arg → `$HOME`, `-` → `$OLDPWD`   |
| `pwd`    | no options                                                  |
| `export` | `KEY=val`, `KEY`, `KEY+=val`; sorted `declare -x` listing   |
| `unset`  | silently skips invalid names, like bash                     |
| `env`    | prints variables that have a value                          |
| `exit`   | `exit [n]`, numeric check, `too many arguments`             |

### Technical choices

```
line ──► lexer ──► syntax check ──► parser (+ expansion) ──► heredocs ──► executor
         tokens     `|` / redir      t_cmd list: argv +        temp files    builtin in parent
                    errors → $?=2    t_redir list              (forked)      or fork/pipe/execve
```

- **Lexer:** single left-to-right pass; quotes are kept inside words so that
  `|`, `<`, `>` and spaces inside quotes are not treated as operators.
- **Parser:** the mandatory grammar is flat (`cmd | cmd | ... | cmd`), so the
  token list is simply split on pipes into a linked list of commands — no AST
  or recursion needed.
- **Expander:** small quote state machine; unquoted expansions are word-split,
  quoted ones stay a single word, like Bash.
- **Signals:** a single `static volatile sig_atomic_t` global stores only the
  signal number; the main loop reads it and updates `$?`.
- **Non-interactive mode:** when stdin is not a terminal, no prompt is printed
  (same as Bash), and lines are read with a small `read`-based reader.

| Directory        | Role                                                         | Author   |
|------------------|--------------------------------------------------------------|----------|
| `src/core`       | main loop, non-tty line reader, errors, cleanup              | both     |
| `src/lexer`      | tokenizer and syntax validation                              | nachebbi |
| `src/expander`   | `$VAR` / `$?` expansion, quote removal, word splitting       | nachebbi |
| `src/parser`     | tokens → command list with redirections                      | nachebbi |
| `src/executor`   | pipelines, redirections, PATH resolution, heredocs           | akolomii |
| `src/builtins`   | the seven builtins                                           | both     |
| `src/env`        | environment linked list                                      | akolomii |
| `src/signals`    | signal handlers                                              | akolomii |
| `libft`          | own minimal libft                                            | both     |

## Instructions

### Requirements

- `cc` and `make`
- GNU readline: `sudo apt install libreadline-dev` on Debian/Ubuntu,
  `brew install readline` on macOS (the Makefile picks it up automatically; the
  system readline on macOS is libedit and lacks `rl_clear_history` /
  `rl_replace_line`)

### Compilation

```bash
make            # build ./minishell
make clean      # remove objects
make fclean     # remove objects + binary
make re         # rebuild
```

### Execution

```bash
./minishell                       # interactive shell
echo 'echo hello | wc -c' | ./minishell   # non-interactive
env -i ./minishell                # start with an empty environment
```

### Checking for leaks

```bash
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
         --trace-children=yes ./minishell
```

readline's own allocations are not required to be freed by the subject.
Detailed per-folder documentation and the testers live on the `test` branch.

## Resources

### References

- [GNU Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html) — the reference behaviour
- [POSIX Shell Command Language](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html) — tokenization, quoting, expansion order
- [GNU Readline Library](https://tiswww.case.edu/php/chet/readline/readline.html)
- `man 2 fork`, `man 2 execve`, `man 2 pipe`, `man 2 dup2`, `man 2 waitpid`, `man 2 sigaction`, `man 7 signal`
- [Writing Your Own Shell — Purdue CS252](https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf)
- [The Linux Programming Interface](https://man7.org/tlpi/) — processes, pipes and signals chapters
- [LucasKuhn/minishell_tester](https://github.com/LucasKuhn/minishell_tester) — community tester

### Use of AI

AI (Claude, by Anthropic) was used as an assistant during the project, for:

- **Explanations:** concepts such as `volatile sig_atomic_t` and signal
  handling, the lexer/parser design, the `_` environment variable, and Bash
  behaviour in edge cases.
- **Debugging and testing:** diagnosing why the community tester failed on
  some machines, generating edge-case commands to compare with Bash, and
  running the shell under valgrind to look for leaks and open file descriptors.
- **Repository maintenance:** moving the project to the repository root,
  Makefile fixes for macOS readline, norminette-related cleanup of the signal
  global, and drafting documentation including this README.

All code was reviewed, tested and understood by the team before being kept.