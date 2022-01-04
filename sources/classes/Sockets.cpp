#include "webserv.hpp"

namespace ws{

Sockets::Sockets( void ){
}
	
		
Sockets::~Sockets( void ){
	_pollfds.clear();
}

void		Sockets::add_client( int new_socket ){
	struct pollfd	elem;
	t_client		c_elem;
	
	elem.fd = new_socket;
	elem.events = POLLIN;
	_pollfds.push_back(new_socket);
	c_elem.fd = new_socket;
	_clients.push_back(c_elem);
}
}