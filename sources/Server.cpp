#include "Server.hpp"

namespace ws{
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server( void ) {}
Server::Server( std::string ip) : _server_ip(ip) {}
Server::Server( const Server & src ) { *this = src; }

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
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

void	Server::launchServer( void )
{
	for (std::vector<Port>::iterator it = _ports.begin(); it != _ports.end(); it++)
	{
		if ((*it).launchPort())
			addToPolling((*it).getFd());
		else
		{
			//GESTION D'ERROR//
		}
	}
	while (true)
	{
		_clean_fds = 0;
		setRevents();
		if (int ret = polling() <= 0)
			break;
		for (it_port pt = _ports.begin(); pt != _ports.end(); pt++)
		{
			int fd = 0;
			while ((fd = (*pt).accepting()) != -1)
				addToPolling(fd);
		}
		for (it_port pt = _ports.begin(); pt != _ports.end(); pt++)
		{
			for (it_client ct = (*pt).getClients().begin(); ct != (*pt).getClients().end();)
			{
				//std::cout << "REVENTS = " << findFds((*ct).getFd()).revents << std::endl;
				if ((findFds((*ct).getFd()).revents) == 0)
				{
					ct++;
					continue;
				}
				else if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLIN)))
				{
					//std::cout << "FD = " << (*ct).getFd() << std::endl;
					//std::cout << findFds((*ct).getFd()).revents << std::endl;
					//std::cout << (findFds((*ct).getFd()).revents & POLLIN) << std::endl;
					int ret = (*ct).receive();
					if ( ret == WRITING)
					{
						(findFds((*ct).getFd())).events = POLLOUT;
						(*ct).executeCGI();
					}
					else if (ret == ERROR)
					{
						//ERROR//
					}
					ct++;
				}
				else if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLOUT)))
				{
					int ret = (*ct).send();
					//(findFds((*ct).getFd())).events = POLLOUT;
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
				else //GERER FLAG D'ERREURS DE POLL POUR EVITER BOUCLE INFINIE
				{
					ct++;
				}
				// findFds((*ct).getFd()).revents = 0;
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
	new_elem.revents = 0;
	_fds.push_back(new_elem);
}

void		Server::setRevents( void )
{
	for (it_fds it = _fds.begin(); it != _fds.end(); it++)
		(*it).revents = 0;
}



/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string			Server::getIp( void ) const { return (this->_server_ip); }
std::vector<Port>	Server::getPorts( void ) const { return (this->_ports); }
std::vector<Port>&	Server::getRefPorts( void ) {return this->_ports;}
void				Server::setIp( std::string new_ip ) { this->_server_ip = new_ip; }
void				Server::setPorts( std::vector<Port> new_ports) { this->_ports = new_ports; }
}
/* ************************************************************************** */
