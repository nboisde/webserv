#include "listenSocket.hpp"

ws::listenSocket::listenSocket() : Socket() {};
ws::listenSocket::~listenSocket() {};

int	ws::listenSocket::listenning()
{
	if (listen(server_fd, 3) < 0)
	{
		perror("In listen\n");
		exit(EXIT_FAILURE);
	}
	std::cout << "listen succeeded\n";
	return (1);
}

int	ws::listenSocket::accepting()
{
	int			new_socket;
	socklen_t	addrlen = sizeof(address);

	new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	if (new_socket < 0)
	{
		perror("In accept");
		exit(EXIT_FAILURE);
	}
	std::cout << "accept succeeded\n";
	return (new_socket);
}