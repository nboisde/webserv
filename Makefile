NAME		= webserver
CC			= clang++
RM			= rm -rf
FLAGS		= -Wall -Werror -Wextra -std=c++98
SRCS_DIR	= sources
INC_DIR		= includes

all			init ${NAME}

${NAME}:	init
			${CC} ${FLAGS} -i${INC_DIR} *.cpp

clean:
			${RM} ${OBJS_DIR}

fclean:		clean
			${RM} a.out

re:			fclean

.SUFFIXWZ	.cpp .o .hpp
.PHONY		all clean fclean re