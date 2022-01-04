#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include "webserv.hpp"

class Port;

namespace ws{
class Server
{

	public:

		Server( void );
		Server( std::string, std::vector<Port> );
		Server( Server const & src );
		virtual ~Server( void );

		Server &		operator=( Server const & rhs );

		std::string			printPorts( void ) const;
		std::string			getIp( void ) const;
		std::vector<Port>	getPorts( void ) const;
		void				setIp( std::string new_ip );
		void				setPorts( std::vector<Port> new_ports);
		void				addPort( Port new_port);

	protected:

		std::string			_server_ip;
		std::vector<Port>	_ports;
};
}

std::ostream &			operator<<( std::ostream & o, ws::Server const & i );

#endif /* ********************************************************** SERVER_H */