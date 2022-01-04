#ifndef __LISTENINGSOCKET_HPP
# define LISTENINGSOCKET_HPP

# include "webserv.hpp"

namespace ws
{
class listenSocket : public ws::Socket
{
	public:
		listenSocket();
		virtual ~listenSocket();

		int listenning();
		int	accepting();
};
};

#endif
