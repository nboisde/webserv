#include "webserv.hpp"
#include "Server.hpp"
#include "Parser.hpp"

int	main(int ac, char **av)
{
	ws::Parser	parser;
	
	if (ac != 2)
	{
		std::cout << "Missing valid config file\n";
		return (1);
	}
	if (!parser.launch(av[1]))
	{
		std::cout << "Wrong config" << std::endl;
		return (1);
	}
	parser.getServer().launchServer();
	return (0);
}
