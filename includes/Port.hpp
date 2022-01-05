#ifndef __PORTS_HPP__
# define __PORTS_HPP__

# include "webserv.hpp"
# include "Sockets.hpp"

class Sockets;

namespace ws{
class Port
{

	public:

		Port( int port_socket );
		Port( Port const & src );
		virtual ~Port( void );

		Port &		operator=( Port const & rhs );

		int							bind( void );
		int							listening( void );
		int							accepting( void );
		int							getFd( void ) const;
		int							getPort( void ) const;
		struct sockaddr_in			getPortAddress( void ) const;
		std::vector<Client>		getClients( void ) const;
		void						addClient( int fd );

	protected:
		Port( void );
		int 					_fd;
		int						_port;
		struct sockaddr_in		_port_address;
		std::vector<Client>	_clients;
};

}

std::ostream &			operator<<( std::ostream & o, ws::Port const & i );

#endif /* ****************************************************** PORTSCLASS_H */