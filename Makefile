# Source program name
NAME	= ft_ssl

# Source program compiler settings
CC		= cc
FLAGS	= -Wall -Wextra -Werror -g -D VERBOSE=$(V)

# Directories
BLDD	= build
SRCD	= src
INCD	= include

# Source compile variables
SRCS	= $(shell find $(SRCD) -type f -name "*.c")
OBJS	= $(patsubst %.c, $(BLDD)/%.o, $(SRCS))
INCS	= -I $(SRCD) -I $(INCD)
DEPS	= $(OBJS:.o=.d)

# Fake targets
.PHONY: clean fclean re norm \
		san valgrind clear

MAKEFLAGS += --no-print-directory

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $^ -o $@ $(INCS)

$(BLDD)/%.o: %.c
	@ mkdir -p $(@D)
	$(CC) $(FLAGS) -MMD -c $< -o $@ $(INCS)

-include $(DEPS)

clean:
	rm -rf $(BLDD)

fclean: clean
	rm -rf $(NAME)

re: fclean all

san: FLAGS += -g -fsanitize=address,leak,undefined
san: all

valgrind:
	valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all ./$(NAME) $(F)

clear:
	rm -rf $(BLDD)
