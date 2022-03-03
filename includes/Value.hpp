#ifndef VALUE_HPP
# define VALUE_HPP

# include "webserv.hpp"

namespace ws
{

class	Route
{
	public:
		std::string					route;
		std::string					rewrite;
		std::string					autoindex;
		std::string					index;
		std::string					upload;
		std::vector<std::string>	methods;

		Route() : route(""), rewrite(""), autoindex("off"), index(""), upload("") {};
		~Route() {};
		Route( Route const & src );
		Route &		operator=( Route const & rhs ); 
		
};

class Value
{
	public:
		Value();
		Value( std::string value );
		Value( Value const & src );
		virtual ~Value();

		Value &		operator=( Value const & rhs );

		bool								_default;
		std::string							_value;
		unsigned long						_max_body_size;
		std::map<int, std::string>			_errors;
		std::vector<std::string>			_methods;
		std::map<std::string, Route>		_locations;
		//ONLY USED BY CGI, BASED ON OLD ARCHI
		std::map<std::string, std::string>	_list;
};

std::ostream &			operator<<( std::ostream & o, Value const & i );
}
#endif /* *********************************************************** VALUE_H */
