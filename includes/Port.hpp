#ifndef __PORT_HPP__
# define __PORT_HPP__

# include "Client.hpp"

namespace ws
{

class Client;

class Port
{
	typedef	std::vector<std::string>	method_type;
	typedef	std::map<int, std::string>	error_type;

	public:
		Port( void );
		Port( int port );
		Port( Port const & src );
		virtual ~Port( void );

		Port &				operator=( Port const & rhs );

		int					launchPort( void );
		int					bind( void );
		int					listening( void );
		int					accepting( void );
		void				addClient( int fd );
		void				removeClient( int fd );
	
		int					getFd( void ) const ;
		void				setFd( int fd);
		int					getPort( void ) const;
		void				setPort( int fd);
		std::string			getHost( void ) const;
		void				setHost( std::string );
		bool				getAutoindex( void ) const;
		void				setAutoindex( bool on );
		std::string			getServerName( void ) const;
		void				setServerName( std::string );
		int					getClientMaxSize( void ) const;
		void				setClientMaxSize( int size );
		struct sockaddr_in	getPortAddress( void ) const;
		std::vector<Client>	&getClients( void );
		void				setClients( int fd );

	protected:
		bool				_autoindex;
		int 				_fd;
		int					_port;
		int					_client_max_size;
		std::string			_host;
		std::string			_server_name;
		method_type			_methods;
		error_type			_errors;
		struct sockaddr_in	_port_address;
		std::vector<Client>	_clients;
};

}

std::ostream &				operator<<( std::ostream & o, ws::Port const & i );

#endif
