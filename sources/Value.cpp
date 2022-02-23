#include "Value.hpp"

namespace ws
{

Value::Value( void ) : _max_body_size(-1) {}
Value::Value( std::string value) : _value(value), _max_body_size(-1) {}
Value::Value( Value const & src ) { *this = src; }
Value::~Value( void ) {}

Value & Value::operator=( Value const & rhs ) 
{w
	if (this != &rhs)
	{
		_value = rhs._value;
		_max_body_size = rhs._max_body_size;
		_errors = rhs._errors;
		_methods = rhs._methods;
		_list = rhs._list;
		_locations = rhs._locations;
	}
	return *this;
}

Route::Route( Route const & src ) { *this = src; }

Route & Route::operator=( Route const & rhs ) 
{
	if (this != &rhs)
	{
		redirection = rhs.redirection;
		autoindex = rhs.autoindex;
		index = rhs.index;
		upload = rhs.upload;
		methods = rhs.methods;
	}
	return *this;

std::ostream &			operator<<( std::ostream & o, Value const & i )
{
	o << i._value;
	return o;
}

}
