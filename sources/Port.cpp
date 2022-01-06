#include "webserv.hpp"

namespace ws{
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Port::Port( void )
{
	return;
}

Port::Port( int port ) : _port(port)
{
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
		this->_port = rhs.getPort();
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

int		Port::launchPort( void ){
	int on = 1;

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
	{
		perror("In socket: ");
		return ERROR;
	}
	if ((setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
	{
		perror("In setsockopt: ");
		close(_fd);
		return ERROR;
	}
	if ((fcntl(_fd, F_GETFL, O_NONBLOCK)) < 0)
	{
		perror("In fcntl: ");
		return ERROR;
	}
	memset((char *)&_port_address, 0, sizeof(_port_address));
	_port_address.sin_family = AF_INET; 
	_port_address.sin_addr.s_addr = htonl(INADDR_ANY); 
	_port_address.sin_port = htons(_port);
	if (bind() < 0 || listening() < 0)
		return ERROR;
	return SUCCESS;
}


int	Port::bind( void ){
	if (::bind(_fd, (struct sockaddr *)&_port_address, sizeof(_port_address)) < 0)
	{
		perror("In bind");
		close(_fd);
		return ERROR;
	}
	return SUCCESS;
}

int Port::listening( void )
{
	if (listen(_fd, 3) < 0)
	{
		perror("In listen\n");
		return ERROR;
	}
	return SUCCESS;
}

int	Port::accepting( void )
{
	int					new_socket;
	struct sockaddr_in	cli_addr;
	socklen_t			addrlen = sizeof(cli_addr);

	new_socket = accept(_fd, (struct sockaddr *)&cli_addr, (socklen_t*)&addrlen);
	if (new_socket < 0)
	{
		perror("In accept");
		exit(ERROR);
	}
	// Client	newClient(new_socket);
	// //CREER NOUVEAU CLIENT, ET L'AJOUTER A LA LISTE DE CLIENTS DU PORT
	// _clients.push_back(newClient);
	return (new_socket);
}


void	removeClient( int fd )
{
	(void)fd;
	// std::vector<Client>::iterator it = _clients.begin();
	// std::vector<Client>::iterator ite = _clients.end();

	// for (it; it != ite; it++)
	// {
	// 	if (fd == (*it).fd)
	// 	{
	// 		close(fd);
	// 		pollfd[i].fd = -1;
	// 	}
	// }
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */
}