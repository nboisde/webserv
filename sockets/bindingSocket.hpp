#ifndef BINDINGSOCKET_HPP
# define BINDINGSOCKET_HPP

#include "basicSocket.hpp"

namespace WS{
	class bindingSocket: public basicSocket{
		public:
			bindingSocket(int domain, int socket, int protocol, int port, unsigned long interface);
			~bindingSocket();

			int networkConnection(int sock, struct sockaddr_in address);
	};
};

#endif
