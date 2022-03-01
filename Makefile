
NAME				= webserv
CC					= clang++
RM					= rm -rf
CFLAGS				= -Wall -Werror -Wextra -std=c++98
IFLAGS				= -I${INC_DIR}
SRCS_DIR			= sources
INC_DIR				= includes
OBJS_DIR			= objects
LOGS_DIR			= logs

INC					= $(shell find ${INC_DIR} -type f -name "*.hpp")
SRCS 				= $(notdir $(shell find ${SRCS_DIR} -type f -name "*.cpp"))
OBJS 				= $(addprefix ${OBJS_DIR}/, $(SRCS:.cpp=.o))
vpath				%.cpp $(shell find ${SRCS_DIR} -type d)


all:				init ${NAME}

init:		
					$(shell mkdir -p ${OBJS_DIR})
					$(shell mkdir -p ${LOGS_DIR})

${NAME}:			${OBJS}
					${CC} ${CFLAGS} ${IFLAGS} -o $@ $^

${OBJS_DIR}/%.o:	%.cpp
					@echo "${CLEAR}"
					@echo "compiling $<\r\c"
					@${CC} ${CFLAGS} ${IFLAGS} -c $< -o $@

clean:
					${RM} ${OBJS_DIR}
					${RM} ${LOGS_DIR}/access.log

fclean:				clean
					${RM} ${NAME}

re:					fclean all

.SUFFIXES:			.cpp .o .hpp
.PHONY:				all clean fclean re

