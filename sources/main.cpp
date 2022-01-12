#include "webserv.hpp"
#include "Server.hpp"
#include "parsing.hpp"

int	main(int ac, char **av)
{
	ws::Server server;

	if (ac != 2)
		return (1);
	if (!parsing(av[1], &server))
	{
		std::cout << "Wrong config" << std::endl;
		return (1);
	}
	server.getPortsNb().push_back(8080);
	server.launchServer();
	return (0);
}
