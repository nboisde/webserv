#include "Port.hpp"

namespace ws{
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Port::Port( void ) {}
Port::Port( std::map<std::string, std::string> dictionnary) : _config(dictionnary) {}
Port::Port( const Port & src ) { *this = src; }

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
		_fd = rhs._fd;
		_port_address = rhs._port_address;
		_clients = rhs._clients;
		_config = rhs._config;
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Port const & i )
{
	o << "Port_nb = " << (i.getConfig())["listen"];
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
	if ((fcntl(_fd, F_SETFL, O_NONBLOCK)) < 0)
	{
		perror("In fcntl: ");
		return ERROR;
	}
	memset((char *)&_port_address, 0, sizeof(_port_address));
	_port_address.sin_family = AF_INET; 
	_port_address.sin_addr.s_addr = htonl(INADDR_ANY); 
	_port_address.sin_port = htons(atoi((_config["listen"]).c_str()));
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
	if (listen(_fd, 5) < 0)
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
		if (errno != EAGAIN)
			perror("In accept");
		return(ERROR);
	}
	Client newClient(new_socket);
	_clients.push_back(newClient);
	return (new_socket);
}


void	Port::removeClient( int fd )
{
	std::vector<Client>::iterator it = _clients.begin();

	for (; it != _clients.end(); it++)
	{
		if (fd == (*it).getFd())
		{
			close(fd);
			//it = _clients.erase(it);
			return;
		}
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int										Port::getFd( void ) const { return _fd; }
void									Port::setFd( int fd ) {	_fd = fd; }
std::vector<Client>	&					Port::getClients( void ) { return _clients; }
std::map<std::string, std::string>		Port::getConfig( void ) const { return _config; }
std::map<std::string, std::string> &	Port::getConfig( void ) { return _config; }

/* ************************************************************************** */
}
