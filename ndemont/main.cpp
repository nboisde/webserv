#include "Socket.hpp"
#include "listenSocket.hpp"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

# define BUFFER 1000

struct pollfd &new_pollfd(int socketfd) //PROTEGER MEMOIRE
{
	struct pollfd *ptr = new struct pollfd;
	ptr->fd = socketfd;
	ptr->events = POLLIN;
	return (*ptr);
}

int	main()
{
	ws::listenSocket	listenSocket;
	int					connectSocket_fd;
	char				*hello = (char *)("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8;\r\nContent-Length: 21\r\n\r\n<h1>Hello world!</h1>\n");
	int					delai = -1;
	std::vector<struct pollfd>	pollfd;

	listenSocket.listenning();
	while(1)
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		
		if ((connectSocket_fd = listenSocket.accepting()) < 0)
		{
			perror("accepting failed");
			continue;
		}
		pollfd.push_back(new_pollfd(connectSocket_fd));

		std::vector<struct pollfd>::iterator it = pollfd.begin();
		std::vector<struct pollfd>::iterator ite = pollfd.end();
		poll(&(*pollfd.begin()), pollfd.size(), delai);
		for(; it != ite; it++)
		{
			
			if ((it->revents & POLLIN) == POLLIN)
			{
				std::cout << connectSocket_fd << " set for reading" << std::endl;
				char buffer[BUFFER] = {0};
				recv(it->fd, buffer, BUFFER, 0);
				std::cout << buffer << std::endl;
			}
			if ((it->revents & POLLIN) != POLLIN)
			{
				send(it->fd , hello,strlen(hello), 0);
				close(it->fd);
				//pollfd.erase(it);
			}
		}
	}
	close(listenSocket.server_fd);
	return (0);
}