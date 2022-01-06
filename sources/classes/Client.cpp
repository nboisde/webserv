#include "webserv.hpp"

namespace ws{

Client::Client(){}

Client::Client( int fd ) : _fd(fd){}
Client::Client( Client & src ){}
Client::~Client(){}

int Client::receive(){}
int Client::send(){}
void Client::closeConnection(){}

int Client::getFd(void) const{
	return _fd;
}
Request Client::getReq(void) const{
	return _req;
}

}