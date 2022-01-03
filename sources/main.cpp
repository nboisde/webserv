#include "socket.hpp"
#include "server.hpp"

int	main()
{
	printf("%d\n", POLLHUP);
	while (1)
	{
		launch_server();
		std::cout << "\n++++++++++++ Relaunch server +++++++++++++\n\n";
	}
	return (0);
}