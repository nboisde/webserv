#ifndef __CGI_HPP__
# define __CGI_HPP__

# include <webserv.hpp>
# include "Client.hpp"

namespace ws
{

class Client;

class CGI {
	public:

		CGI( Client const & cli );
		CGI( CGI const & src );
		virtual ~CGI( void );

		CGI &		operator=( CGI const & rhs );

		void								init_conversion( void );

		int									generate_env( void );
		int									generate_arg( void );
		int									execute( Client & cli );


		int									createResponse(int fd, std::string & response);

		std::string							getBinLocation( void ) const;
		std::map<std::string, std::string>	getHeader( void ) const;

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