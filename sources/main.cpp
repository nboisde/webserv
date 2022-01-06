#include "Server.hpp"

int	main()
{
	//CREER PARSER DE CONFIG ET PASSER LE RESULTAT DANS LE CONSTRUCTEUR DE SERVER.//
	//parser(av[1])
	ws::Server server;
	server.getPortsNb().push_back(8080);
	server.launchServer();
	return (0);
}