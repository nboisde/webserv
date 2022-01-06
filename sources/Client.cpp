#include "Client.hpp"

namespace ws
{

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/ 
Client::Client( void ) {}

Client::Client( int fd ) : _fd(fd) {}

Client::Client( Client const & src ) 
{
	*this = src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Client::~Client() {}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Client &				Client::operator=( Client const & rhs )
{
	if ( this != &rhs )
	{
		this->_fd = rhs.getFd();
		this->_status = rhs.getStatus();
		this->_req = rhs._req;
		this->_res = rhs._res;
	}
	return *this;
}


int Client::receive(int fd)
{
	char	buffer[BUFFER_SIZE];

	for (size_t i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = 0;
	int ret = recv(fd, buffer, sizeof(buffer), 0);
	_req.getRawContent() = _req.concatenateRequest((std::string)buffer);
	if (ret < 0)
	{
		perror("\nIn recv");
		return ERROR;
	}
	if (ret < BUFFER_SIZE)
		_status = WRITING;
	return SUCCESS;
}


int Client::send( void )
{
	int ret;

	if ((ret = ::send(_fd, _res.getResponse().c_str(), BUFFER_SIZE, 0)) < 0)
	{
		perror("  send() failed");
		return (ERROR);		
	}
	_res.getResponse() += ret;
	if (_res.getResponse().empty())
		_status = CLOSING;
	return (SUCCESS);
}

void Client::closeConnection(){}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	Client::getStatus( void ) const
{
	return _status;
}


int Client::getFd(void) const
{
	return _fd;
}

Request & Client::getReq( void )
{
	return _req;
}

Response & Client::getRes( void )
{
	return _res;
}

}