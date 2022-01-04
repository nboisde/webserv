#include "webserv.hpp"

int launch_server(void);


int	main()
{
	while (1)
	{
		launch_server();
		std::cout << "\n++++++++++++ Relaunch server +++++++++++++\n\n";
	}
	return (0);
}