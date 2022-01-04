#ifndef WEBSERV_HPP
# define WEBSERV_HPP

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
# include "Server.hpp"
# include "requests.hpp"
# include "socket.hpp"
# include "listenSocket.hpp"
# include "Port.hpp"
# include "Poll.hpp"



#endif