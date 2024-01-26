# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: qthierry <qthierry@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/31 18:39:31 by jvigny            #+#    #+#              #
#    Updated: 2024/01/16 20:55:26 by qthierry         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = rope_simu

CC = gcc
CFLAGS = -Wall -Wextra -MMD -g#-O3#-fsanitize=address,leak,undefined #-Werror
LIBS = -L$(MINILIBX_DIR) -lmlx -lX11 -lXext -lm 
INCLUDES = -I$(MINILIBX_HEADERS) -I$(HEADERS_DIR)

REDIRECT_ERROR = >/dev/null 2>&1

RESET_COLOR	:= \033[0m
RED			:= \033[0;31m
GREEN		:= \033[0;32m

HEADERS_DIR = ./includes/

# ------------------MLX-------------------
MINILIBX_URL = https://github.com/42Paris/minilibx-linux.git
MINILIBX_DIR = $(HEADERS_DIR)minilibx-linux/
MINILIBX_HEADERS = $(MINILIBX_DIR)
MINILIBX = $(MINILIBX_DIR)libmlx.a
# ----------------------------------------

SRC_LIST =	\
			hooks.c \
			main.c

SRC_DIR = ./src/
SRC = $(addprefix $(SRC_DIR),$(SRC_LIST))

OBJ_DIR = ./obj/
OBJ_LIST = $(patsubst %.c,%.o, $(SRC_LIST))
OBJ = $(addprefix $(OBJ_DIR), $(OBJ_LIST))
DEP_LIST = $(patsubst %.o,%.d, $(OBJ))

all:	$(NAME)

bonus:	all

-include $(DEP_LIST)

$(NAME):	$(MINILIBX) $(RAUDIO) $(OBJ_DIR) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) $(INCLUDES) -o $(NAME)

$(OBJ_DIR)%.o:	$(SRC_DIR)%.c Makefile
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR): Makefile
	@mkdir -p $(OBJ_DIR)

$(MINILIBX):
	@echo "${GREEN}~-~-~-~-~ COMPILING MINILIBX ~-~-~-~-~${RESET_COLOR}"
	@echo "   ${GREEN}- Fetching sources...${RESET_COLOR}"
	@if [ ! -d "$(MINILIBX_DIR)" ]; then \
		git clone $(MINILIBX_URL) $(MINILIBX_DIR); \
	else \
		cd $(MINILIBX_DIR) && git pull; \
	fi
	@echo "   ${GREEN}- Compiling sources...${RESET_COLOR}"
	@make -s -C $(MINILIBX_DIR) all $(REDIRECT_ERROR)
	@echo "${GREEN}~ DONE ~\n${RESET_COLOR}"

clean_minilibx:
	rm -rf $(MINILIBX_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean:	clean clean_minilibx
	rm -rf $(NAME)

re: fclean all

.PHONY : all bonus clean fclean re run vrun prun clean_minilibx
