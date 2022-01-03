#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <cstdio>
# include <iostream>
# include <sys/types.h> 
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <functional>
# include <fcntl.h>
# include <sys/select.h>
# include <unistd.h>
# include <sys/time.h>
# include <poll.h>
# include <vector>

namespace ws
{
class Socket
{
	public:
		struct sockaddr_in	address;
		int					server_fd;

		Socket();
		virtual ~Socket();

		int bindSocket();
};
};


int launch_server(void);
#endif