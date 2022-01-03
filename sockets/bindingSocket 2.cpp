#include "bindingSocket.hpp"

WS::bindingSocket::bindingSocket(int domain, int socket, int protocol, int port, unsigned long interface): basicSocket(domain, socket, protocol, port, interface){
	int connection = networkConnection(basicSocket::getSock(), basicSocket::getAaddress());
	testConnection(connection);
}
WS::bindingSocket::~bindingSocket(){}

int WS::bindingSocket::networkConnection(int sock, struct sockaddr_in address){
	// Ici, changer le type de cast.
	return bind(sock, (struct sockaddr *)(&address), sizeof(address));
}

