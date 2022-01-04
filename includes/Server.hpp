#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"

class Port;
class Poll;

namespace ws{
class Server
{

	public:

		Server( void );
		Server( std::string, std::vector<Port> );
		Server( Server const & src );
		virtual ~Server( void );

		Server &		operator=( Server const & rhs );

		std::string			getIp( void ) const;
		std::vector<Port>	getPorts( void ) const;
		void		setIp( std::string new_ip );
		void		setPorts( std::vector<Port> new_ports);

	protected:

		std::string			_server_ip;
		std::vector<Port>	_ports;
};
}

std::ostream &			operator<<( std::ostream & o, ws::Server const & i );

#endif /* ********************************************************** SERVER_H */