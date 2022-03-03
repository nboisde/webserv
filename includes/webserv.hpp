#ifndef __WEBSERV_HPP__
# define __WEBSERV_HPP__

// General MACROS
# define DELAI -1
# define BUFFER_SIZE 4096
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
# define MULTIPART 3

// Connection handling
# define CLOSE 0
# define KEEP_ALIVE 1

// RESPONSE TYPES

# define R_EXT 1
# define R_HTML 2
# define R_ERR 3
# define R_AUTO 4
# define R_REDIR 5
# define R_UPLOAD 6
# define R_DELETE 7

// CLIENT STATUS
# define CLOSING 0
# define READING 1
# define WRITING 2

# define STDIN 0
# define STDOUT 1
# define STDERR 2

// UNITS
# define GIGA 1000000000
# define MEGA 1000000
# define KILO 1000

// Methods MACROS
# define UNKNOWN 0
# define GET 1
# define POST 2
# define DELETE 3

// RESPONSE HEADER STATUS
# define CGI_FLAG 42
# define CONTINUE 100
# define OK 200
# define NO_CONTENT 202
# define MULTIPLE_CHOICE 300
# define MOVED_PERMANETLY 301
# define BAD_REQUEST 400
# define UNAUTHORIZED 401
# define FORBIDDEN 403
# define NOT_FOUND 404
# define NOT_ALLOWED 405
# define REQUEST_ENTITY_TOO_LARGE 413
# define INTERNAL_SERVER_ERROR 500

// HOSTNAMES
# define LOCALHOST "localhost"

// COLORS
# define RED "\033[38;5;124m"
# define RESET "\033[0m"
# define BLUE "\033[38;5;63m"
# define GREEN "\033[38;5;42m"
# define FIRE "\033[38;5;166m"
# define PURPLE "\033[38;5;105m"
# define LIGHTBLUE "\033[38;5;87m"
# define YELLOW "\033[38;5;220m"
# define DEV "\033[38;5;200m"

//CPP_LIBRARIES//
# include <fstream>
# include <utility>
# include <iostream>
# include <cstdlib>
# include <string>
# include <sstream>
# include <vector>
# include <map>
# include <csignal>
# include <algorithm>
# include <functional>


//C_LIBRARIES//#include <string.h>
# include <string.h>
# include <stdlib.h>
# include <errno.h>
# include <stdlib.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <net/if.h>
# include <netinet/in.h>
# include <poll.h>
# include <unistd.h>
# include <stdio.h>
# include <fcntl.h>
# include <stdio.h>
# include <dirent.h>

//# include <sys/event.h>
# include <sys/ioctl.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/select.h>


// Function prototypes.

class SignalEnd : public std::exception
{
    public:
        SignalEnd(){};
        SignalEnd( int nb) : _sig_number(nb){};
        virtual const char * what() const throw(){return ("Gracefully shutting down <3");};
        int getSignal( void ){return _sig_number;};
    private:
        int _sig_number;
};

void	signalHandler(int signum);

#endif
