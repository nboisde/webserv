#include "Server.hpp"

namespace ws{
/*
** ------------------------------- CONSTRUCTOR / DESTRUCTOR ------------------------
*/

Server::Server( void ) {}
Server::Server( std::string ip) : _server_ip(ip) {}
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
	o << "Server IP = " << i.getIp() << std::endl;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

static char *trim(char *str)
{
	char *s = str;
	int i = 0;
	int end = strlen(s);
	while (isspace(*s))
	{
		i++;
		s++;
	}
	if (i == end)
		return NULL;
	end--;
	while (isspace(s[end]))
		end--;
	if (end == -1)
		return NULL;
	else
		s[end + 1] = '\0';
    return (s); 
}

static int	monitor_stdin( void )
{
	int exit_status = 0;
	FILE * stream = fdopen(dup(STDIN), "r");
	char *line = NULL;
	size_t n;
	if (stream == NULL)
	{	
		perror("fopen");
		return (1);
	}
	ssize_t ret = getline(&line, &n, stream);
	fclose(stream);
	if (ret == -1 || line == NULL || trim(line) == NULL)
	{
		if (errno && errno != EAGAIN)
			perror("getline");
		exit_status = 1;
	}
	if (!exit_status && !(strcmp(trim(line), "exit")))
		exit_status = 0;
	else
		exit_status = 1;
	if (line)
		free(line);
	return (exit_status);
}

void	Server::launchServer( void )
{
	addToPolling(STDIN);
	for (std::vector<Port>::iterator it = _ports.begin(); it != _ports.end();)
	{
		if ((*it).launchPort() > 0)
		{
			addToPolling((*it).getFd());
			it++;
		}
		else
			_ports.erase(it);
	}
	if (!_ports.size())
		exit(1);
	while (true)
	{
		_clean_fds = 0;
		setRevents();
		if (int ret = polling() <= 0)
			break;
		if (findFds(STDIN).fd == 0 && (findFds(STDIN).revents & POLLIN) && !(monitor_stdin()))
				return ;
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
				else if (findFds((*ct).getFd()).fd != 0 && ((findFds((*ct).getFd()).revents & POLLERR)))
				{
					std::cout << RED<< "Client socket fd : " << findFds((*ct).getFd()).fd << " raised an error." << std::endl;
 					closeConnection(ct, pt);
				}
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

					int ret = (*ct).receive();

					if ( ret == WRITING)
					{
						(findFds((*ct).getFd())).events = POLLOUT;
						(*ct).execution(*this, *pt);
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

void	Server::closeConnection(it_client & ct, it_port & pt)
{
	int tempo = (*ct).getFd();
	findFds(tempo).revents = 0;
	findFds(tempo).fd = -1;
	((*pt).getClients()).erase(ct);
	//(*pt).removeClient(tempo);
	close(tempo);
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
}
/* ************************************************************************** */
