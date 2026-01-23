# ========

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -ggdb3
LFLAGS	= -lft
LDFLAGS	= -L$(LIBFT)

# ========

OBJS	= $(SRCS:.c=.o)
SRCS	= ./ft_nm.c \
		  ./ft_nm-x32.c \
		  ./ft_nm-x64.c \
		  ./ft_nm-util.c \
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


TEST_FILES	= $(TARGET)
TEST_FLAGS	=

.PHONY : test

test : all
	@-diff <(nm $(TEST_FLAGS) $(TEST_FILES)) <(./$(TARGET) $(TEST_FLAGS) $(TEST_FILES))


# ========

$(TARGET) : $(OBJS)
	make -C $(LIBFT) bonus
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LFLAGS)


$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<


# ========
