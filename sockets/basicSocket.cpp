#include "basicSocket.hpp"

WS::basicSocket::basicSocket(int domain, int service, int protocol, int port, unsigned long interface): domain(domain), service(service), protocol(protocol){
	// Defining address structure.
	this->address.sin_family = this->domain;
	this->address.sin_port = htons(port);
	this->address.sin_addr.s_addr = htonl(interface);

	// Establish connection via socket.
	this->sock = socket(this->domain, this->service, this->protocol);
	this->connection = networkConnection(this->sock, this->address);
	testConnection(this->connection);
}

WS::basicSocket::~basicSocket(){}

// Confirm that connection is well established.
void	WS::basicSocket::testConnection(int item_to_test){
	if (item_to_test < 0)
	{
		perror("Failed to connect");
		exit(EXIT_FAILURE);
	}
}

// Getters
struct sockaddr_in WS::basicSocket::getAddress() const{ return this->address; }
int WS::basicSocket::getSock() const{ return this->sock; }
int WS::basicSocket::getConnection() const{ return this->connection; }
