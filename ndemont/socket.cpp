#include "socket.hpp"

ws::Socket::Socket()
{
	const int	PORT = 8080;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = htonl(INADDR_ANY); 
	address.sin_port = htons(PORT); 
}

ws::Socket::~Socket() {};

int	ws::Socket::bindSocket()
{
	if (bind(server_fd,(struct sockaddr *)&address,sizeof(address)) < 0)
	{
		perror("bind failed\n");
		return (0); 
	}
	perror("bind succeeded\n");
	return (1);
}