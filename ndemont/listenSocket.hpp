#ifndef LISTENINGSOCKET_HPP
# define LISTENINGSOCKET_HPP

# include "Socket.hpp"

namespace ws
{
class listenSocket : public ws::Socket
{
	private:

	public:
		listenSocket();
		virtual ~listenSocket();

		int Listen();
};
};

#endif

