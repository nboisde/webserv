#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

namespace ws
{
	class Response {
		public:
			Response();
			~Response();

			std::string		getResponse( void ) const;
			
		private:
            std::string		_response;
	};
}

#endif