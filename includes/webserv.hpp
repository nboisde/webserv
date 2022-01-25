#ifndef __WEBSERV_HPP__
# define __WEBSERV_HPP__

// General MACROS
# define DELAI -1
# define BUFFER_SIZE 80
# define ERROR -1
# define SUCCESS 1

// Litteral MACROS
# define CRLF "\r\n"
# define BODY_CRLF "\r\n\r\n"

// Requests State MACROS
# define REQUEST_FORMAT_ERROR -1
# define RECEIVING_HEADER 0
# define HEADER_RECEIVED 1
# define BODY_RECEIVED 2

// Body reception method MACROS
# define BODY_RECEPTION_NOT_SPECIFIED 0
# define CONTENT_LENGTH 1
# define TRANSFER_ENCODING 2

// Connection handling.
# define CLOSE 0
# define KEEP_ALIVE 1

// RESPONSE TYPES
# define R_CGI 1
# define R_HTML 2

// CLIENT STATUS
# define CLOSING 0
# define READING 1
# define WRITING 2

# define STDIN 0
# define STDOUT 1
# define STDERR 2

// Methods MACROS
# define UNKNOWN 0
# define GET 1
# define POST 2
# define DELETE 3

// RESPONSE HEADER STATUS
# define COMTINUE 100
# define OK 200
# define MULTIPLE_CHOICE 300
# define MOVED_PERMANETLY 301
# define BAD_REQUEST 400
# define UNAUTHORIZED 401
# define NOT_ALLOWED 405
# define NOT_FOUND 404
# define INTERNAL_SERVER_ERROR 500

//CPP_LIBRARIES//
# include <fstream>
# include <utility>
# include <iostream>
# include <cstdlib>
# include <string>
# include <sstream>
# include <vector>
# include <map>

//C_LIBRARIES//#include <string.h>
# include <string.h>
# include <stdlib.h>
# include <errno.h>
# include <stdlib.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/select.h>
# include <poll.h>
# include <sys/types.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>

//# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <stdio.h>

#endif
