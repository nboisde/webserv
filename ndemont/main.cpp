#include "Socket.hpp"
#include "listenSocket.hpp"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>


int	main()
{
	ws::listenSocket	listenSocket;
	int					connectSocket;
	char				*hello = (char *)("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!");

	listenSocket.listenning();
	while(1)
	{

		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		connectSocket = listenSocket.accepting();
		if (connectSocket < 0)
		{
			perror("In accept");
			exit(EXIT_FAILURE);
		}
		char buffer[30000] = {0};
		int valread = read(connectSocket, buffer, 30000);
		if (valread < 0)
		{
			perror("In read");
			exit(EXIT_FAILURE);
		}
		printf("%s\n", buffer);
		write(connectSocket , hello, strlen(hello));
		printf("------------------Hello message sent-------------------\n");
		close(connectSocket);
	}
	return (0);
}