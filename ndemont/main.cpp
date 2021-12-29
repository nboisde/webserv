#include "Socket.hpp"
#include "listenSocket.hpp"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>


int	main()
{
	ws::listenSocket	L_socket;
	int					C_socket;

	L_socket.listenning();
	C_socket = L_socket.accepting();


	char buffer[1024] = {0};
	int valread = read(C_socket, buffer, 1024); 
	printf("%s\n",buffer );
	if(valread < 0)
	{ 
		printf("No bytes are there to read");
	}
	char *hello = (char *)("Hello from the server\n");//IMPORTANT! WE WILL GET TO IT
	write(C_socket, hello , strlen(hello));
	return (0);
}