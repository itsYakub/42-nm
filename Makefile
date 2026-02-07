# ========

SHELL	= /usr/bin/zsh

# ========

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -ggdb3
LFLAGS	= -lft
LDFLAGS	= -L$(LIBFT)

# ========

OBJS	= $(SRCS:.c=.o)
SRCS	= ./ft_nm.c 		\
		  ./ft_nm-file.c	\
		  ./ft_nm-print.c	\
		  ./ft_nm-sort.c	\
		  ./ft_nm-x64.c 	\
		  ./ft_nm-x32.c 	\
		  ./ft_nm-ar.c 		\
		  ./ft_nm-util.c 	\
		  ./ft_nm-getopt.c

# ========

LIBFT	= ./libft/

# ========

TARGET	= ./ft_nm

# ========


.PHONY : all

all : $(TARGET)


.PHONY : re

re : fclean all


.PHONY : fclean

fclean : clean
	make -C $(LIBFT) fclean
	rm -f $(TARGET)


.PHONY : clean

clean :
	make -C $(LIBFT) clean
	rm -f $(OBJS)


TEST_FLAGS	=

.PHONY : test

test : all
	@-diff <(/bin/nm $(TEST_FLAGS)) <($(TARGET) $(TEST_FLAGS))


.PHONY : tests

tests : all
	$(info Test: no flags)
	@make -C . test
	
	$(info Test: -a, --debug-syms)
	@make -C . test TEST_FLAGS=-a
	
	$(info Test: -g, --extern-only)
	@make -C . test TEST_FLAGS=-g
	
	$(info Test: -u, --undefined-only)
	@make -C . test TEST_FLAGS=-u
	
	$(info Test: -r, --reverse-sort)
	@make -C . test TEST_FLAGS=-r
	
	$(info Test: -p, --no-sort)
	@make -C . test TEST_FLAGS=-p

# ========

$(TARGET) : $(OBJS)
	make -C $(LIBFT) bonus
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LFLAGS)


$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<


# ========
