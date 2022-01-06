#include "webserv.hpp"

int	main()
{
	//CREER PARSER DE CONFIG ET PASSER LE RESULTAT DANS LE CONSTRUCTEUR DE SERVER.//
	//parser(av[1])
	ws::Server server;
	server.launchServer();
	return (0);
}