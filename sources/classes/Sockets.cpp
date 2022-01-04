#include "webserv.hpp"

namespace ws{

Sockets::Sockets( void ){
}
	
		
Sockets::~Sockets( void ){
	_pollfds.clear();
}

void		Sockets::close_connection( int i );
{
	ret = send(pollfd[i].fd, hello, strlen(hello), 0);
	if (ret < 0)
	{
		perror("  send() failed");
		close_conn = 1;
		return 1;
	}
	close(pollfd[i].fd);
	pollfd[i].fd = -1;
	return 0;
}

void		Sockets::add_socket( int new_socket ){
	struct pollfd	elem;
	t_client		c_elem;
	
	elem.fd = new_socket;
	elem.events = POLLIN;
	_pollfds.push_back(new_socket);
	c_elem.fd = new_socket;
	_clients.push_back(c_elem);
}

void		Sockets::erase_socket( void ){
	for (int i = 0; i < (int)pollfd.size(); i++)
	{
		if (pollfd[i].fd == -1)
		{
			pollfd.erase(pollfd.begin() + i);
			clients.erase(clients.begin() + i);
		}
	}
}

}