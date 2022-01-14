#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include "Port.hpp"

class Client;
class Port;

namespace ws{
class Server
{

	typedef std::vector<struct pollfd>::iterator	it_fds;
	typedef std::vector<Port>::iterator 			it_port;
	typedef std::vector<Client>::iterator			it_client;

	public:

		Server( void );
		Server( std::string ip );
		Server( Server const & src );
		virtual ~Server( void );

		Server &		operator=( Server const & rhs );

		std::string					printPorts( void ) const;
		std::string					getIp( void ) const;
		std::vector<Port>			getPorts( void ) const;
		std::vector<Port>&			getRefPorts( void );
		std::vector<struct pollfd>	getFds( void ) const;
		void						setIp( std::string new_ip );
		void						setPorts( std::vector<Port> new_ports);
		void						setFds( std::vector<struct pollfd> fds);
		void						addPort( Port new_port);
		int							polling( void );
		void						addToPolling( int fd );
		void						setRevents( void );
		void						cleanFds( void ) ;
		struct pollfd	&			findFds( int fd );
		void						launchServer( void );
		int							stopServer( void );

	protected:
		bool							_clean_fds;
		std::string						_server_ip;
		std::string						_server_name;
		std::vector<Port>				_ports;
		std::vector<struct pollfd>		_fds;

};
}

std::ostream &			operator<<( std::ostream & o, ws::Server const & i );

#endif /* ********************************************************** SERVER_H */
