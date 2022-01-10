#include "Server.hpp"

namespace ws{
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server( void )
{
	return;
}

Server::Server( std::string ip, std::vector<Port> ports) : _server_ip(ip), _ports(ports)
{
	return;
}

Server::Server( const Server & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server( void )
{
	_ports.clear();
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Server &				Server::operator=( Server const & rhs )
{
	if ( this != &rhs )
	{
		this->_server_ip = rhs.getIp();
		this->_ports = rhs.getPorts();
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Server const & i )
{
	o << "Server IP = " << i.getIp() << std::endl;
	o << "Access through ports : " << i.printPorts() << std::endl;                                                                
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

void	Server::launchServer( void )
{
	for (std::vector<int>::iterator it = _ports_nb.begin(); it != _ports_nb.end(); it++)
	{
		Port new_port(*it);
		if (new_port.launchPort())
		{
			_ports.push_back(new_port);
			addToPolling(new_port.getFd());
		}
		else
		{
			//GESTION D'ERROR//
		}
	}
	while (true)
	{
		_clean_fds = 0;
		//setEvents();
		if (int ret = polling() <= 0)
		{	
			//std::cout << "RETURN POLLING " << ret << std::endl;
			break;
		}
		for (it_port pt = _ports.begin(); pt != _ports.end(); pt++)
		{
			int fd;
			while ((fd = (*pt).accepting()) != -1)
			{
				//std::cout << "New Connection " << fd << std::endl;
				addToPolling(fd);
			}
		}
		for (it_port pt = _ports.begin(); pt != _ports.end(); pt++)
		{
			for (it_client ct = (*pt).getClients().begin(); ct != (*pt).getClients().end();)
			{
				if ((findFds((*ct).getFd()).revents) == 0)
				{
					ct++;
					continue;
				}
				if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLIN)))
				{
					//std::cout << "FD = " << (*ct).getFd() << std::endl;
					//std::cout << findFds((*ct).getFd()).revents << std::endl;
					//std::cout << (findFds((*ct).getFd()).revents & POLLIN) << std::endl;
					int ret = (*ct).receive();
					if ( ret == WRITING)
						(findFds((*ct).getFd())).events = POLLOUT;
					else if (ret == ERROR)
					{
						//ERROR//
					}
					ct++;
				}
				else if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLOUT)))
				{
					int ret = (*ct).send();
					if (ret == CLOSING)
					{	
						int tempo = (*ct).getFd();
						findFds(tempo).revents = 0;
						findFds(tempo).fd = -1;
						((*pt).getClients()).erase(ct);
						//(*pt).removeClient(tempo);
						close(tempo);
						_clean_fds = 1;
					}
					else
						ct++;
				}
			}
		}
		if (_clean_fds)
			cleanFds();
	}
}

struct pollfd & Server::findFds( int fd)
{
	std::vector<struct pollfd>::iterator it = _fds.begin();
	std::vector<struct pollfd>::iterator ite = _fds.end();

	for (; it != ite; it++)
	{
		if ((*it).fd == fd)
			return (*it);
	}
	return *ite;
}

void	Server::cleanFds( void )
{
	for (it_fds it = _fds.begin(); it != _fds.end(); )
	{
		if ((*it).fd == -1)
			it = _fds.erase(it);
		else
			it++;
	}
}

int		Server::stopServer( void ){
	return (1);
}

std::string	Server::printPorts( void ) const{
	std::vector<Port>::const_iterator it, itend; 
	std::string ret;
	
	it = _ports.begin();
	itend = _ports.end();
	if (it == itend)
	{
		return "No Ports in memory";
	}
	for (; it != itend; it++)
	{
		ret += (*it).getPort();
		if (it != itend)
			ret += " ";
	}
	return ret;
}

void		Server::addPort( Port new_port )
{
	this->_ports.push_back(new_port);
}

int			Server::polling( void )
{
	return (poll(&(*_fds.begin()), _fds.size(), DELAI));
}

void		Server::addToPolling( int fd )
{
	struct pollfd new_elem;
	new_elem.fd = fd;
	new_elem.events = POLLIN;
	_fds.push_back(new_elem);
}

void		Server::setEvents( void )
{
	for (it_fds it = _fds.begin(); it != _fds.end(); it++)
	{
		(*it).events = POLLIN;
		for (it_port pt = _ports.begin(); pt != _ports.end(); pt++)
		{
			for (it_client ct = (*pt).getClients().begin(); ct != (*pt).getClients().end(); ct++)
			{
				if ((*ct).getFd() == (*it).fd)
				{
					(*it).events = (*ct).getReq().getState();
				}
			}
		}
	}
}



/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string	Server::getIp( void ) const{
	return (this->_server_ip);
}

std::vector<Port> Server::getPorts( void ) const{
	return (this->_ports);
}

void	Server::setIp( std::string new_ip ) {
	this->_server_ip = new_ip;
}

void	Server::setPorts( std::vector<Port> new_ports) {
	this->_ports = new_ports;
}

std::vector<int>&	Server::getPortsNb( void )
{
	return _ports_nb;
}


}
/* ************************************************************************** */
