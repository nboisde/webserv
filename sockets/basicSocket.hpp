#ifndef BASICSOCKET_HPP
# define BASICSOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>

namespace WS{
	class basicSocket{
		public:
			//constructors and destructors
			basicSocket(int domain, int service, int protocol, int port, unsigned long interface);
			~basicSocket();

			//virtual funtion to connect to a network;
			virtual int networkConnection(int sock, struct sockaddr_in address) = 0;
			//test socket and connection.
			void testConnection(int item_to_test);
			
			//getters
			struct sockaddr_in getAddress() const;
			int getSock() const;
			int getConnection() const;
		private:
			int domain;
			int service;
			int protocol;
			int connection;
			int sock;
			struct sockaddr_in address;
	};
}

#endif