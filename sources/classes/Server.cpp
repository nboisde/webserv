#include "webserv.hpp"

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

void	Server::launchServer( void ){
	for (std::vector<int>::iterator it = _ports_nb.begin(); it != _ports_nb.end(); it++)
	{
		ws::Port new_port(*it);
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
		setEvents();
		if (polling() <= 0)
		{	
			break;
			/*ERROR*/
		}
		for (it_fds it = _fds.begin(); it != _fds.end(); it++)
		{
			for (it_port pt = _ports.begin(); pt != _ports.end(); pt++)
			{
				for (it_client ct = (*pt).getClients().begin(); ct != (*pt).getClients().end(); ct++)
				{
					if ((*it).revents == 0)
						continue;
					else if ((*it).fd == (*pt).fd)
					{
						while (int fd = (*it).accepting() != -1)
							addToPolling(fd);
					}
					else if ((*ct).getReq().getState() == 0 || (*ct).getReq().getState() == 1)
						(*ct).receive();
					else if ((*ct).getReq().getState() == 2)
						(*ct).send();
				}
			}
		}
		if (remove_connection)
			cleanFds();
	}
}

void	Server::cleanFds( void )
{
	for (it_fds it = _fds.begin(); it != _fds.end(); it++)
	{
		if ((*it).fd == -1)
			_fds.erase(it);
	}
}

int		Server::stopServer( void ){

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
	if (new_port)
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
}
/* ************************************************************************** */
