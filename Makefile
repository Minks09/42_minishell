NAME		= minishell

CC			= cc
CFLAGS		= -Wall -Wextra -Werror

# Readline: macOS ships libedit (no rl_clear_history/rl_replace_line),
# so use GNU readline from Homebrew there. Linux: libreadline-dev.
UNAME_S		:= $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
RL_PREFIX	?= $(shell brew --prefix readline 2>/dev/null)
ifeq ($(RL_PREFIX),)
$(error GNU readline not found: run "brew install readline")
endif
RL_INC		?= -I$(RL_PREFIX)/include
RL_LIB		?= -L$(RL_PREFIX)/lib -lreadline
else
RL_INC		?=
RL_LIB		?= -lreadline
endif

LIBFT_DIR	= libft
LIBFT		= $(LIBFT_DIR)/libft.a

INCLUDES	= -I include -I $(LIBFT_DIR) $(RL_INC)

SRC_DIR		= src
OBJ_DIR		= obj

SRCS		= main.c \
			  core/shell.c \
			  core/read_line.c \
			  core/error.c \
			  core/cleanup.c \
			  lexer/lexer.c \
			  lexer/syntax.c \
			  expander/expand.c \
			  expander/expand_var.c \
			  expander/expand_utils.c \
			  parser/parser.c \
			  parser/redir.c \
			  executor/exec.c \
			  executor/pipeline.c \
			  executor/redirections.c \
			  executor/path.c \
			  executor/heredoc.c \
			  executor/heredoc_utils.c \
			  builtins/builtin.c \
			  builtins/bi_echo.c \
			  builtins/bi_cd.c \
			  builtins/bi_pwd_env.c \
			  builtins/bi_export.c \
			  builtins/bi_export_print.c \
			  builtins/bi_exit.c \
			  env/env.c \
			  env/env_utils.c \
			  signals/signals.c \
			  utils/utils.c

OBJS		= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))
DEPS		= $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(RL_LIB) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR) bonus

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf $(OBJ_DIR)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
