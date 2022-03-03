#include "Value.hpp"

namespace ws
{

Value::Value( void ) : _default(false), _max_body_size(-1) {}
Value::Value( std::string value) : _default(false), _value(value), _max_body_size(-1) {}
Value::Value( Value const & src ) { *this = src; }
Value::~Value( void ) {}

Value & Value::operator=( Value const & rhs ) 
{
	if (this != &rhs)
	{
		_default = rhs._default;
		_value = rhs._value;
		_max_body_size = rhs._max_body_size;
		_errors = rhs._errors;
		_methods = rhs._methods;
		_locations = rhs._locations;
		_list = rhs._list;
	}
	return *this;
}

Route::Route( Route const & src ) { *this = src; }

Route & Route::operator=( Route const & rhs ) 
{
	if (this != &rhs)
	{
		route = rhs.route;
		rewrite = rhs.rewrite;
		autoindex = rhs.autoindex;
		index = rhs.index;
		upload = rhs.upload;
		methods = rhs.methods;
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Value const & i )
{
	o << i._value;
	return o;
}

}
