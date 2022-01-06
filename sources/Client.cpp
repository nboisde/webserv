#include "webserv.hpp"

namespace ws{

Client::Client(){}

Client::Client( int fd ) : _fd(fd){}
Client::Client( Client & src ){}
Client::~Client(){}

int Client::receive()
{
	char	buffer[BUFFER_SIZE];

	for (size_t i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = 0;
	ret = recv(pollfd[i].fd, buffer, sizeof(buffer), 0);
	_rq = clients[i].r.concatenateRequest((std::string)buffer);
	if (ret < 0)
	{
		perror("\nIn recv");
		return ERROR;
	}
	if (ret < BUFFER_SIZE)
		_status = WRITING;
	return SUCCESS;
}

int	Client::getStatus()
{
	return _status;
}

int Client::send( void )
{
	int ret;

	if ((ret = send(_fd, _res, BUFFER_SIZE, 0) < 0)
	{
		perror("  send() failed");
		return (ERROR);		
	}
	_res += ret;
	if (!_res)
		_status = CLOSING;
	return (SUCCESS);
}

void Client::closeConnection(){}

int Client::getFd(void) const{
	return _fd;
}
Request Client::getReq(void) const{
	return _req;
}

}