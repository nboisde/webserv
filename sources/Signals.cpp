#include "webserv.hpp"

void	signalHandler(int signum)
{
	std::cout << std::endl << GREEN << "Closing the server..." << RESET << std::endl;
	sleep(1);	
	std::cout << GREEN << "Bye bye" << RESET << std::endl;
	throw SignalEnd(signum);
}
