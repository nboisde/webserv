#ifndef VALUE_HPP
# define VALUE_HPP

# include "webserv.hpp"

namespace ws
{

typedef struct	s_route
{
	std::string					redirection;
	std::string					autoindex;
	std::string					index;
	std::string					upload;
	std::vector<std::string>	method;
	std::vector<std::string>	keys;
	s_route() : redirection(""), autoindex("off"), index(""), upload("")
	{
		keys.push_back("upload");
		keys.push_back("redirection");
		keys.push_back("autoindex");
		keys.push_back("index");
		keys.push_back("method");
	}
}				route;

class Value
{
	public:
		Value();
		Value( std::string value );
		Value( Value const & src );
		virtual ~Value();

		Value &		operator=( Value const & rhs );

		std::string						_value;
		unsigned long					_max_body_size;
		std::map<int, std::string>		_errors;
		std::vector<std::string>		_methods;
		std::map<std::string, route>	_locations;
};

std::ostream &			operator<<( std::ostream & o, Value const & i );
}
#endif /* *********************************************************** VALUE_H */
