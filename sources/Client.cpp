#include "Client.hpp"

namespace ws
{

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/ 
Client::Client( void ) {}

Client::Client( int fd ) : _fd(fd), _status(WRITING) {}

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

int Client::receive(void)
{
	char	buffer[BUFFER_SIZE];

	for (size_t i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = 0;
	int ret = recv(_fd, buffer, BUFFER_SIZE - 1, 0);
	/*std::cout << "RECEIVED CONTENT = [" << buffer << "]" << std::endl;
	std::cout << "[";
	for (int i = 0; i < ret; i++)
	{
		std::cout << "|" << (int)buffer[i];
	}
	std::cout << "]" << std::endl;*/
	int req = _req.concatenateRequest((std::string)buffer);
	if ( ret < 0 )
	{
		perror("\nIn recv");
		return WRITING;
	}
	if (/* ret < BUFFER_SIZE - 1 || */req == 1)
	{
		//std::cout << "ret :" << ret << std::endl;
		_req.fillHeaderAndBody();
		std::cout << _req.getRawContent() << std::endl;
		std::cout << "--------------------------------" << std::endl << "Header:" << std::endl;
		std::cout << _req.getHeader() << std::endl;
		std::cout << "Body : " << std::endl << _req.getBody() << std::endl;
		//std::cout << _req.getBody();// << std::endl;
		return WRITING;
	}
	return READING;
}


int Client::send( void )
{
	int			ret;
	int			len;
	const char* prov;
	std::string str = _res.getResponse();	

	len = str.size();
	prov = str.c_str();
	if (len > BUFFER_SIZE)
		len = BUFFER_SIZE;
	//std::cout << "SEND FD = " << _fd << std::endl;
	ret = ::send(_fd, prov, len, 0);
	//std::cout << "RET IN SENDING " << ret << std::endl;
	if (ret < 0)
	{
		perror(" send() failed");
		return (ERROR);		
	}
	str.erase(0, ret);
	if (str.empty())
		return CLOSING;
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