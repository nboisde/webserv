#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

#include "Request.hpp"
#include "Response.hpp"

class Request;
class Response;



namespace ws {
class Client{
	public:
		Client( void );
		Client( int fd );
		Client( Client const & src );
		virtual ~Client();
			
		Client &	operator=( Client const & rhs );

		int 		receive( void );
		int			send( void );
		void		closeConnection( void );

		int			getFd( void ) const;
		int			getStatus( void ) const;
		Request &	getReq( void );
		Response &	getRes( void );

	private:
		int			_fd;
		int			_status;
		Request		_req;
		Response	_res;
};
}

#endif