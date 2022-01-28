#include "webserv.hpp"
#include "Server.hpp"
#include "Parser.hpp"

int	main(int ac, char **av)
{
	ws::Parser	parser;
	std::ifstream f("./conf/title.txt");

	std::signal(SIGINT, signalHandler);
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
	if (f.is_open())
	{
		std::cout << FIRE << f.rdbuf() << RESET << std::endl << std::endl;
		f.close();
	}
	parser.getServer().launchServer();
	return (0);
}
