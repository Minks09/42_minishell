NAME	= libft.a

CC		= cc
CFLAGS	= -Wall -Wextra -Werror

SRCS	= ft_str.c \
		  ft_str2.c \
		  ft_mem.c \
		  ft_char.c \
		  ft_conv.c \
		  ft_put.c \
		  ft_split.c \
		  ft_lst.c
OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)

%.o: %.c libft.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
