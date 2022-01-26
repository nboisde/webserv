#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include "webserv.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "CGI.hpp"
# include "Server.hpp"
# include "Port.hpp"

namespace ws {

class Port;

class Client
{
	protected:
		int				_fd;
		int				_status;
		ws::Request		_req;
		ws::Response	_res;
		std::string		_file_path;

	public:
		Client( void );
		Client( int fd );
		Client( Client const & src );
		virtual ~Client();
			
		Client &		operator=( Client const & rhs );

		int				checkURI( Port & port );
		int				checkCGI( int query_pos );
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
		ws::Request &	getReq( void );
		ws::Response &	getRes( void );
};

}

#endif
