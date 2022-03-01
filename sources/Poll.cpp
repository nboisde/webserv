#include "Server.hpp"

namespace ws{

void    Server::initializeSockets(void)
{
    for (it_port port = _ports.begin(); port != _ports.end();)
    {
        if ((*port).launchPort() > 0)
        {
            addToPolling((*port).getFd());
            port++;
        }
        else
            _ports.erase(port);
    }
    if (!_ports.size())
        exit(1);
}

void    Server::acceptConnections(void)
{
    for (it_port pt = _ports.begin(); pt != _ports.end(); pt++)
    {
        int fd = 0;
        while ((fd = (*pt).accepting()) != -1)
            addToPolling(fd);
    }
}

void	Server::launchServer( void )
{
    //createSocket and initialize them
    initializeSockets();

    //launch polling and monitoring of sockets
	while (true)
	{
		_clean_fds = 0;
		setRevents();

		if (polling() <= 0)
			break; 

        //accept incoming connections, and add them to polling list
		acceptConnections();

        //ITER on ALL PORTS TO CHECK ALL THEIR SOCKETS
		for (it_port pt = _ports.begin(); pt != _ports.end(); pt++)
		{
            //ITER ON ALL SOCKETS/CLIENTS
			for (it_client ct = (*pt).getClients().begin(); ct != (*pt).getClients().end();)
			{
				if(!ct->getFileFlag())
				{
					std::cout << "PENDIIIIIIIING" << std::endl;
					goto pending;
				}
				if ((findFds((*ct).getFd()).revents) == 0)
				{ 
					ct++;
					continue;
				}
				else if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLERR)))
				{
					std::cout << RED << "Client socket fd : " << findFds((*ct).getFd()).fd << " raised an error." << RESET << std::endl;
 					closeConnection(ct, pt);
				}
#ifdef __linux__
				else if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLRDHUP)))
#else
				else if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLHUP)))
#endif
				{
					std::cout << LIGHTBLUE << "Client " << findFds((*ct).getFd()).fd << " closed the connection." << RESET << std::endl;
 					closeConnection(ct, pt);
				}
				else if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLIN)))
				{
					pending:
					std::cout << "OK" << std::endl;
					int ret = (*ct).receive();
					std::cout << "BRAVO " << ret << std::endl;
					if ( ret == WRITING)
					{
						//WILL ENTER AS MANY TIME NEEDED TO COMPLETE FILE WRITING
						ct->execution(*this, *pt);

						//IF FILE COMPLETION, SETTING FLAG 
						bool	file_completion = ct->getFileFlag();
						if (file_completion)
						{
							std::cout << YELLOW << "JAMAIS 2 FOIS" << RESET << std::endl;
							(findFds((*ct).getFd())).events = POLLOUT;
						}
					}
					else if (ret == ERROR)
					{
						//ERROR//
					}
					ct++;
				}
				else if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLOUT)))
				{
					std::cout << DEV << "CACA\n" << RESET;
					int ret = (*ct).send();
					if (ret == CLOSING)
					{
						if ((*ct).getReq().getConnection() == CLOSE)
						{
							std::cout << LIGHTBLUE << "Client " << findFds((*ct).getFd()).fd << " closed the connection." << RESET << std::endl;
 							closeConnection(ct, pt);
						}
						else
						{
							(*ct).getReq().resetValues();
							(*ct).resetValues();
							findFds((*ct).getFd()).revents = 0;
# ifdef __linux__
							findFds((*ct).getFd()).events = POLLIN | POLLRDHUP | POLLERR;
# else
							findFds((*ct).getFd()).events = POLLIN | POLLHUP | POLLERR;
#endif
						}
					}
					else
						ct++;
				}
				else
					ct++;
			}
		}
		if (_clean_fds)
		{
			std::cout << "PRETTY SURE I PASS HERE" << std::endl;
			cleanFds();
		}
	}
}
}