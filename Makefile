NAME=pipex

CUR:=$(shell pwd)

SRCS_PATH=$(CUR)/srcs
OBJS_PATH=$(CUR)/objs
HEADER_PATH=$(CUR)/include
LIBFT_PATH=$(CUR)/libft
LIBFT_A=$(LIBFT_PATH)/libft.a

SRC_FILES=main.c parse_cmd.c parse_cmd_utils.c execute_child.c parse_cmd_empty_space.c handle_errors.c free_utils.c waitpid_utils.c

SRCS=$(addprefix $(SRCS_PATH)/,$(SRC_FILES))
OBJS=$(addprefix $(OBJS_PATH)/,$(SRC_FILES:.c=.o))
DEPS=$(addprefix $(OBJS_PATH)/,$(SRC_FILES:.c=.d))

CC=cc
CFLAGS=-Wall -Wextra -Werror
DEPFLAGS=-MMD -MP

all: init $(NAME)

init:
	@mkdir -p $(OBJS_PATH)

$(NAME): $(OBJS) $(LIBFT_A)
	@echo "generating object files + dependency files in root"
	@echo "generating pipex file in root"
	$(CC) $(CFLAGS) $^ -o $@

$(LIBFT_A):
	@echo "generating libft.a" 
	@$(MAKE) -C $(LIBFT_PATH)

$(OBJS_PATH)/%.o: $(SRCS_PATH)/%.c | init
	$(CC) $(CFLAGS) $(DEPFLAGS) -I$(HEADER_PATH) -I$(LIBFT_PATH) -c $< -o $@

clean:
	@echo "removing object files + dependency files in root"
	@rm -rf $(OBJS_PATH)
	@$(MAKE) -C $(LIBFT_PATH) clean

fclean: clean
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_PATH) fclean

re: fclean all

.PHONY: all clean fclean re init
