#ifndef VALUE_HPP
# define VALUE_HPP

# include "webserv.hpp"

namespace ws
{

typedef struct	s_route
{
	std::string					redir;
	std::string					autoindex;
	std::string					index;
	std::string					upload_path;
	std::vector<std::string>	methods;
	s_route() : redir(0), autoindex("off"), index(0), upload_path(0) {}
}				route;

class Value
{
	public:
		Value();
		Value( std::string value );
		Value( Value const & src );
		virtual ~Value();

		Value &		operator=( Value const & rhs );

		std::string							_value;
		unsigned long						_max_body_size;
		std::map<int, std::string>			_errors;
		std::vector<std::string>			_methods;
		std::map<std::string, std::string>	_locations;
		// std::map<std::string, route>	_locations;
};

std::ostream &			operator<<( std::ostream & o, Value const & i );
}
#endif /* *********************************************************** VALUE_H */
