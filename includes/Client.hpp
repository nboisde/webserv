#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include "webserv.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "CGI.hpp"
# include "Server.hpp"

namespace ws {

//class Request;
//class Response;

class Client
{
	protected:
		int			_fd;
		int			_status;
		ws::Request		_req;
		ws::Response	_res;

	public:
		Client( void );
		Client( int fd );
		Client( Client const & src );
		virtual ~Client();
			
		Client &		operator=( Client const & rhs );

		int 			receive( void );
		int				executeCGI( Server const & serv );
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
