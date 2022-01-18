#ifndef __PORT_HPP__
# define __PORT_HPP__

# include "Client.hpp"

namespace ws
{

class Client;

class Port
{
	typedef	std::map<std::string, std::string>	config_type;
	public:
		Port( std::map<std::string, std::string>);
		Port( Port const & src );
		virtual ~Port( void );

		Port &				operator=( Port const & rhs );

		int					launchPort( void );
		int					bind( void );
		int					listening( void );
		int					accepting( void );
		void				addClient( int fd );
		void				addMethod( std::string );
		void				addError(int error_nb, std::string error_path);
		void				removeClient( int fd );
	
		int					getFd( void ) const ;
		void				setFd( int fd);
		std::vector<Client>	&getClients( void );
		void				setClients( int fd );
		config_type			getConfig( void ) const;

	protected:
		Port( void );

		int					_fd;
		struct sockaddr_in	_port_address;
		std::vector<Client>	_clients;
		config_type			_config;
};

}

std::ostream &				operator<<( std::ostream & o, ws::Port const & i );

#endif
