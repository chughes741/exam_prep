# Special variables
DEFAULT_GOAL: run
.DELETE_ON_ERROR: $(NAME)
.PHONY: all clean fclean
HIDE = @

# Compiler and flags
CC		=	gcc
CFLAGS	=	-Wall -Werror -Wextra -fsanitize=address
RM		=	rm -f

# Output file name
NAME	= mini_serv
SRC		= mini_serv.c

PORT ?= 8080

all: $(NAME)

# Generates output file
$(NAME): $(SRC)
	$(HIDE)$(CC) $(CFLAGS) -o $@ $^

# Removes objects and executables
fclean: clean
	$(HIDE)$(RM) $(NAME)

# Removes objects and executables and remakes
re: fclean all

run: re
	$(HIDE)./$(NAME) $(PORT)

val: all
	$(HIDE)valgrind									\
			--leak-check=full						\
			--show-leak-kinds=all					\
			--show-reachable=yes					\
			--track-fds=yes							\
			--error-limit=no						\
			./$(NAME) $(PORT)
