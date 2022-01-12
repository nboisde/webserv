#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

namespace ws
{
	class Response {
		public:
			Response( void );
			Response( Response const & src );
			virtual ~Response();

			Response &		operator=( Response const & rhs );

			std::string		getResponse( void ) const;
			
		private:
			std::string		_response;
	};
}
#endif
