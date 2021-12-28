#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <cstdio>
# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <functional>

namespace ws 
{
class Socket
{
	private:
		struct sockaddr_in	address;
		int					server_fd;

	public:
		Socket();
		virtual ~Socket();

		int Bind();
};
};
#endif