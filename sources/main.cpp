#include "socket.hpp"
#include "listenSocket.hpp"
#include "server.hpp"

int	main()
{
	while (1)
	{
		launch_server();
		std::cout << "\n++++++++++++ Relaunch server +++++++++++++\n\n";
	}
	return (0);
}