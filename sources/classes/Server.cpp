#include "webserv.hpp"
#include <vector>

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
	//o << "Access through ports : " << i.getPorts() << std::endl;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/


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