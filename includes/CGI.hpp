#ifndef __CGI_HPP__
# define __CGI_HPP__

# include <webserv.hpp>
# include "Client.hpp"
# include "Server.hpp"

namespace ws
{

class Server;
class Client;

class CGI {
	public:

		CGI( Client const & cli, Server const & serv );
		CGI( CGI const & src );
		virtual ~CGI( void );

		CGI &		operator=( CGI const & rhs );

		void								init_conversion( Server const & serv );

		int									generate_env( void );
		int									generate_arg( Client const & cli );
		int									execute( Client & cli );


		std::string							concatenateResponse(int fd);

		std::map<std::string, std::string>	getConversion( void ) const;
		std::string							getBinLocation( void ) const;
		std::map<std::string, std::string>	getHeader( void ) const;
		char **								getArg( void ) const;
		char **								getEnv( void ) const;

	private:

		std::map<std::string, std::string>	_conversion;			
		std::string							_bin_location;
		std::map<std::string, std::string>	_header;
		char								**_arg;
		char 								**_env;
		CGI( void );

};

}

class CGI;

std::ostream &			operator<<( std::ostream & o, CGI const & i );

#endif /* ************************************************************* CGI_H */
