#ifndef CONNECTSOCKET_HPP
# define CONNECTSOCKET_HPP

# include "socket.hpp"

namespace ws
{
class connectSocket : public ws::Socket
{
	public:
		connectSocket();
		virtual ~connectSocket();

		int listenning();
};
};

#endif