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
	//std::cout << "RECEIVED CONTENT = [" << buffer << "]" << std::endl;
	//std::cout << buffer;
	if ( ret < 0 )
	{
		perror("\nIn recv");
		return WRITING;
	}
	std::string tmp(buffer, ret);
	int req = _req.concatenateRequest(tmp);
	if (req == -1)
	{
		std::cout << "Gerer ici une reponse d'erreur" << std::endl;
		return WRITING;
		//exit(EXIT_FAILURE);
	}
	if (/* ret < BUFFER_SIZE - 1 ||  */req == 1)
	{
		//std::cout << "ret :" << ret << std::endl;
		int head_err = _req.fillHeaderAndBody();
		if (head_err == ERROR)
		{
			std::cout << "Gerer ici une reponse d'erreur (parsing du header problematique !) -> Retour d'erreur 400 -> bad request." << std::endl;
			return WRITING;
		}
 		//std::cout << _req.getRawContent() << std::endl;
		//std::cout << "--------------------------------" << std::endl << "Header:" << std::endl;
		std::cout << _req.getHeader() << std::endl;
		//std::cout << "Body : " << std::endl << _req.getBody() << std::endl;
		//std::cout << _req.getBody();// << std::endl;
		//std::cout << _req.getBody().length() << std::endl;
		write(1, _req.getBody().c_str(), _req.getBody().length());
		//std::cout << std::endl;
		return WRITING;
	}
	return READING;
}


int Client::send( void )
{
	int			ret;
	//int			len;
	const char* prov;
	std::string str = _res.response();

	//len = str.size();
	prov = str.c_str();
	ret = ::send(_fd, prov, str.size(), 0);
	if (ret < 0)
	{
		perror(" send() failed");
		return (ERROR);		
	}

	// DEUX CAS A GERER SUR CETTE FONCTION supposement. (avec content-length ou transfer encoding.)
	/*
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
	//str.erase(0, ret - 1);
	if (str.empty())
		return CLOSING;
	//return (WRITING);
	return (SUCCESS);*/
	return CLOSING;
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
