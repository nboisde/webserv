#include "Server.hpp"

namespace ws{
/*
** ------------------------------- CONSTRUCTOR / DESTRUCTOR ------------------------
*/

Server::Server( void ) {}
Server::Server( std::string ip) : _server_ip(ip){}
Server::Server( const Server & src ) { *this = src; }
Server::~Server( void ) { _ports.clear(); }

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Server &				Server::operator=( Server const & rhs )
{
	if ( this != &rhs )
	{
		this->_server_ip = rhs.getIp();
		this->_ports = rhs.getPorts();
		this->_fds = rhs.getFds();
		this->_clean_fds = rhs.getCleanFds();
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Server const & i )
{
	o << i.getIp() << std::endl;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

void	Server::closeConnection(it_client & ct, it_port & pt)
{
	int tempo = (*ct).getFd();
	findFds(tempo).revents = 0;
	findFds(tempo).fd = -1;

	if (tempo >= 0)
		close(tempo);

	if (ct->getCGIFile() != NULL)
	{
		findFds(fileno(ct->getCGIFile())).fd = -1;
		fclose(ct->getCGIFile());
	}
	if (ct->getUploadFd() != -1)
	{
		findFds(ct->getUploadFd()).fd = -1;
		close(ct->getUploadFd());
	}
	((*pt).getClients()).erase(ct);
	_clean_fds = 1;
}

struct pollfd & Server::findFds( int fd )
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
#ifdef __linux__
	new_elem.events = POLLIN | POLLRDHUP | POLLERR;
#else
	new_elem.events = POLLIN | POLLHUP | POLLERR;
#endif
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

bool				Server::getCleanFds( void ) const { return (this->_clean_fds); }
std::string			Server::getIp( void ) const { return (this->_server_ip); }
std::vector<Port>	Server::getPorts( void ) const { return (this->_ports); }
std::vector<struct pollfd>	Server::getFds( void ) const { return (this->_fds); } 
std::vector<Port>&	Server::getRefPorts( void ) {return this->_ports;}
void				Server::setIp( std::string new_ip ) { this->_server_ip = new_ip; }
void				Server::setPorts( std::vector<Port> new_ports) { this->_ports = new_ports; }
void				Server::setCleanFds(bool bl){ this->_clean_fds = bl;}
}
/* ************************************************************************** */
