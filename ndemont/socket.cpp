#include "socket.hpp"

ws::Socket::Socket()
{
	const int	PORT = 8080;
	int flags;
	int on = 1;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		perror("In socket: ");
		exit(EXIT_FAILURE);
	}
	if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
	{
		perror("In setsockopt: ");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	flags = fcntl(server_fd, F_SETFL, 0);
	if ((fcntl(server_fd, F_GETFL, O_NONBLOCK)) < 0)
	{
		perror("In fcntl: ");
		exit(EXIT_FAILURE);
	}
	memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = htonl(INADDR_ANY); 
	address.sin_port = htons(PORT);
}

ws::Socket::~Socket() {}

int	ws::Socket::bindSocket()
{
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("In bind");
		close(server_fd);
		return (0);
	}
	return (1);
}