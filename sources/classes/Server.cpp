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
