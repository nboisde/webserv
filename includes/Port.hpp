#ifndef __PORTS_HPP__
# define __PORTS_HPP__

# include "webserv.hpp"
# include "Sockets.hpp"

class Sockets;

namespace ws{
class Port
{

	public:

		Port( int port_skt );
		Port( Port const & src );
		virtual ~Port( void );

		Port &		operator=( Port const & rhs );

		int					bind( void );
		int					listening( void );
		int					accepting( void );
		int					getNb( void ) const;
		int					getServSocket( void ) const;
		std::vector<Sockets> getClientSocket( void ) const;

	protected:
		Port( void );
		struct sockaddr_in	_address;
		int					_port_skt;
		Sockets 			_client_skt;
};

}

std::ostream &			operator<<( std::ostream & o, ws::Port const & i );

#endif /* ****************************************************** PORTSCLASS_H */