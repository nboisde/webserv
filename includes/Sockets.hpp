#ifndef SOCKETS_HPP
# define SOCKETS_HPP

# include "webserv.hpp"

typedef struct	s_client
{
	int				fd;
	ws::Requests 	r;
	//ws::Response 	ret;
}				t_client;

namespace ws
{
class Sockets
{
	public:
		Sockets( void );
		virtual ~Sockets( void );
	
		//methods:
		void			close_connection( int fd_socket );
		void			add_socket( int new_socket );
		void			erase_socket( int old_socket );

	protected:
		std::vector<struct pollfd>	_pollfds;
		std::vector<t_client>		_clients;
};

}

#endif