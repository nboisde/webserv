#include "Socket.hpp"
#include "listenSocket.hpp"

int	main()
{
	ws::listenSocket	L_socket;
	int					C_socket;

	L_socket.listenning();
	C_socket = L_socket.accepting();
	return (0);
}