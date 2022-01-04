#include "webserv.hpp"

namespace ws{
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Port::Port( void )
{
	return;
}

Port::Port( int port_skt ) : _port_skt(port_skt)
{
	//int flags;
	int on = 1;

	_port_skt = socket(AF_INET, SOCK_STREAM, 0);
	if (_port_skt < 0)
	{
		perror("In socket: ");
		exit(EXIT_FAILURE);
	}
	if ((setsockopt(_port_skt, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
	{
		perror("In setsockopt: ");
		close(_port_skt);
		exit(EXIT_FAILURE);
	}
	//flags = fcntl(_port_skt, F_SETFL, 0);
	if ((fcntl(_port_skt, F_GETFL, O_NONBLOCK)) < 0)
	{
		perror("In fcntl: ");
		exit(EXIT_FAILURE);
	}
	memset((char *)&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET; 
	_address.sin_addr.s_addr = htonl(INADDR_ANY); 
	_address.sin_port = htons(_port_skt);
}

Port::Port( const Port & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Port::~Port( void )
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Port &				Port::operator=( Port const & rhs )
{
	if ( this != &rhs )
	{
		this->_port_nb = rhs.getPort();
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Port const & i )
{
	o << "Port_nb = " << i.getPort();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

int	Port::bind( void ){
	if (bind(_port_skt, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		perror("In bind");
		close(_port_skt);
		return (0);
	}
	return (1);
}

int Port::listening( void )
{
	if (listen(_port_skt, 3) < 0)
	{
		perror("In listen\n");
		exit(EXIT_FAILURE);
	}
	_client_skt.add_socket(_port_skt);
	return (1);
}

void	Port::accepting( void )
{
	int			new_socket;
	struct sockaddr_in cli_addr;
	socklen_t	addrlen = sizeof(cli_addr);

	new_socket = accept(_port_skt, (struct sockaddr *)&cli_addr, (socklen_t*)&addrlen);
	if (new_socket < 0)
	{
		perror("In accept");
		exit(EXIT_FAILURE);
	}
	_client_skt.add_client(new_socket);
	return (new_socket);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	Port::getNb( void ) const{
	return this->_port_nb;
}

int	Port::getServSocket( void ) const{
	return this->_port_skt;
}

Sockets getClientSocket( void ) const{
	return this->_client_skt;
}

}
/* ************************************************************************** */