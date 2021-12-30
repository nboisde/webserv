#include "Socket.hpp"
#include "listenSocket.hpp"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>


int	main()
{
	ws::listenSocket	listenSocket;
	int					connectSocket_fd;
	char				*hello = (char *)("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8;\r\nContent-Length: 21\r\n\r\n<h1>Hello world!</h1>\n");
	fd_set				rset, wset, eset;
	struct timeval tv;
	int					nfds = 0;

	tv.tv_sec = 5;

	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_ZERO(&eset);
	listenSocket.listenning();
	while(1)
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		if ((connectSocket_fd = listenSocket.accepting()) < 0)
		{
			perror("accepting failed");
			continue;
		}
		FD_SET(connectSocket_fd, &rset);
		FD_SET(connectSocket_fd, &wset);
		FD_SET(connectSocket_fd, &eset);
		nfds++;
		std::cout << nfds << std::endl;
		std::cout 
		if (nfds != 0 && select(nfds, &rset, &wset, &eset, &tv) > 0)
		{
			std::cout << "la !!!" << std::endl;
			if (connectSocket_fd < 0)
			{
				perror("In accept");
				exit(EXIT_FAILURE);
			}
			std::cout << "rset: " << FD_ISSET(connectSocket_fd, &rset) << std::cout;
			std::cout << "wset: " << FD_ISSET(connectSocket_fd, &wset) << std::cout;
			std::cout << "eset: " << FD_ISSET(connectSocket_fd, &eset) << std::cout;
			char buffer[30000] = {0};
			int valread = recv(connectSocket_fd, buffer, 30000, 0);
			if (valread < 0)
			{
				perror("In read");
				exit(EXIT_FAILURE);
			}
			printf("%s\n", buffer);
			send(connectSocket_fd , hello, strlen(hello), 0);
			printf("------------------Hello message sent-------------------\n");
			close(connectSocket_fd);
		}
	}
	close(listenSocket.server_fd);
	return (0);
}