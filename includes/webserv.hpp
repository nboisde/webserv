#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// General MACRORS
# define DELAI -1
# define BUFFER_SIZE 4096

// Requests State MACROS
# define REQUEST_FORMAT_ERROR -1
# define RECIEVING_HEADER 0
# define HEADER_RECIEVED 1
# define BODY_RECIEVED 2

// Methods MACROS
# define UNKNOWN 0
# define GET 1
# define POST 2
# define DELETE 3

//C_LIBRARIES//
# include <stdlib.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/select.h>
# include <poll.h>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <stdio.h>

//CPP_LIBRARIES//
# include <string>
# include <iostream>
# include <vector>

//CLASSES//
# include "requests.hpp"
# include "socket.hpp"
# include "listenSocket.hpp"
# include "Port.hpp"
# include "Poll.hpp"
# include "Server.hpp"



#endif