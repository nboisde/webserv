#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include "webserv.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "CGI.hpp"
# include "Server.hpp"
# include "Port.hpp"
# include "Value.hpp"

namespace ws {

class Port;

class Client
{
	typedef	std::map<std::string, Value>	config_type;

	protected:
		int				_fd;
		int				_status;
		std::string		_ip;
		std::string		_port;
		ws::Request		_req;
		ws::Response	_res;
		std::string		_file_path;
		config_type		_config;

	public:
		Client( void );
		Client( int fd, struct sockaddr_in *cli_addr, config_type conf );
		Client( Client const & src );
		virtual ~Client();
			
		Client &		operator=( Client const & rhs );

		int				checkURI( Port & port );
		int				checkCGI( std::string & url );
		void			checkPath( std::string & url, Port & port );
		void			checkExtension( std::string & url, Port & port );
		int				executeCGI( Server const & serv, Port & port );
		int				executeHtml( Port & port );

		int 			receive( void );
		int				send( void );
		void			closeConnection( void );

		int				getFd( void ) const;
		int				getStatus( void ) const;
		ws::Request 	getReq( void ) const ;
		ws::Response	getRes(void ) const;
		ws::Request &	getReq( void );
		ws::Response &	getRes( void );
		std::string		getFilePath( void ) const;
		std::string		getIp( void ) const;
		std::string		getPort( void ) const;
		config_type		getConfig( void ) const;
};

}

#endif
