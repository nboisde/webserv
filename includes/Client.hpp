#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

#include "webserv.hpp"

namespace ws {
	class Client{
		public:
			Client( int fd );
			Client( Client & src );
			~Client();

			int receive();
			int send();
			void closeConnection();

		protected:
			Client();

		private:
			int _fd;
			Requests _req;
			//Response _res;
	};
}

#endif