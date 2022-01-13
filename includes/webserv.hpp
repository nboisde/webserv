#ifndef __WEBSERV_HPP__
# define __WEBSERV_HPP__

// General MACRORS
# define DELAI -1
# define BUFFER_SIZE 80
# define ERROR -1
# define SUCCESS 1

// Requests State MACROS
# define REQUEST_FORMAT_ERROR -1
# define RECIEVING_HEADER 0
# define HEADER_RECIEVED 1
# define BODY_RECIEVED 2

// Body reception method MACROS
# define BODY_RECEPTION_NOT_SPECIFIED 0
# define CONTENT_LENGTH 1
# define TRANSFER_ENCODING 2

// CLIENT STATUS
# define CLOSING 0
# define READING 1
# define WRITING 2

// Methods MACROS
# define UNKNOWN 0
# define GET 1
# define POST 2
# define DELETE 3

//CPP_LIBRARIES//
# include <iostream>
# include <string>
# include <vector>
# include <map>

//C_LIBRARIES//
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/select.h>
# include <poll.h>
# include <sys/types.h>
# include <errno.h>  

// # include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <stdio.h>

//CLASSES//

// # include "Client.hpp"
// # include "Request.hpp"
// # include "Response.hpp"
// # include "Port.hpp"
// # include "Server.hpp"

#endif
