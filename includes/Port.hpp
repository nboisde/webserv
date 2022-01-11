#ifndef __PORT_HPP__
# define __PORT_HPP__

# include "Client.hpp"

namespace ws{
class Port
{
	public:
		Port( void );
		Port( int port );
		Port( Port const & src );
		virtual ~Port( void );

		Port &		operator=( Port const & rhs );

		int							launchPort( void );
		int							bind( void );
		int							listening( void );
		int							accepting( void );
		int							getFd( void ) const ;
		void						setFd( int fd);
		int							getPort( void ) const;
		struct sockaddr_in			getPortAddress( void ) const;
		std::vector<Client>			&getClients( void );
		void						setClients( int fd );
		void						addClient( int fd );
		void						removeClient( int fd );

	protected:
		int 					_fd;
		int						_port;
		struct sockaddr_in		_port_address;
		std::vector<Client>		_clients;
};

}

std::ostream &			operator<<( std::ostream & o, ws::Port const & i );

#endif /* ****************************************************** PORTSCLASS_H */
