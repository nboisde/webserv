#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

#include "webserv.hpp"

class Request;

namespace ws {
	class Client{
		public:
			Client( int fd );
			Client( Client & src );
			~Client();

			int receive();
			int send();
			void closeConnection();

			int getFd(void) const;
			Request getReq(void) const;

		protected:
			Client();

		private:
			int _fd;
			Request _req;
			//Response _res;
	};
}

#endif