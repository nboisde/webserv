#ifndef SOCKETS_HPP
# define SOCKETS_HPP

# include "webserv.hpp"

namespace ws
{
class Sockets
{
	public:
		Sockets( void );
		virtual ~Sockets( void );
	
		//methods:
		void			add_client( int new_socket );

	protected:
		std::vector<struct pollfd>	_pollfds;
};

}

#endif