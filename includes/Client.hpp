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
		Client( Client & src );
		virtual ~Client();

		int 	receive( int fd );
		int		send();
		void	closeConnection();

		int		getFd(void) const;
		Request &	getReq(void);
		int		getStatus( void ) const;

	private:
		int			_fd;
		int			_status;
		Request		_req;
		Response	_res;
};
}

#endif