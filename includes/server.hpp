#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"
# include <iostream>
# include <string>
# include "Ports.hpp"

namespace ws{
class Server
{

	public:

		Server( void );
		Server( std::string, std::vector<Poll> );
		Server( Server const & src );
		virtual ~Server( void );

		Server &		operator=( Server const & rhs );

		std::string	getIp( void ) const;
		std::string getPorts( void ) const;
		void		setIp( std::string new_ip );
		void		setPorts( std::vector<Poll> new_ports);

	protected:

		std::string			_server_ip;
		std::vector<Port>	_ports;
};
}

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */