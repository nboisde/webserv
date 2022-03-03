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
				if(!ct->getFileFlag() || !ct->getCGIFlag())
					goto pending;
				if ((findFds((*ct).getFd()).revents) == 0)
				{ 
					ct++;
					continue;
				}
				else if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLERR)))
 					closeConnection(ct, pt);
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
					int ret = (*ct).receive();
					if ( ret == WRITING)
					{
						//WILL ENTER AS MANY TIME NEEDED TO COMPLETE FILE WRITING
						ct->execution(*this, *pt);

						bool	file_completion = ct->getFileFlag();
						bool	cgi_flag = ct->getCGIFlag();
						if (file_completion && cgi_flag)
							(findFds((*ct).getFd())).events = POLLOUT;
					}
					else if (ret == ERROR)
	 					closeConnection(ct, pt);
					ct++;
				}
				else if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLOUT)))
				{
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
			cleanFds();
	}
}
}