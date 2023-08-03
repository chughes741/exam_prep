# Special variables
DEFAULT_GOAL: run
.DELETE_ON_ERROR: $(NAME)
.PHONY: all clean fclean
HIDE = @

# Compiler and flags
CC		=	gcc
CFLAGS	=	-Wall -Werror -Wextra
RM		=	rm -f

# Output file name
NAME	= mini_serv
SRC		= mini_serv.c

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
	$(HIDE)./$(NAME) 8080
