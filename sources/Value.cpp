#include "Value.hpp"

namespace ws
{

Value::Value( void ) : _max_body_size(-1) {}
Value::Value( std::string value) : _value(value), _max_body_size(-1) {}
Value::Value( Value const & src ) { *this = src; }
Value::~Value( void ) {}

Value & Value::operator=( Value const & rhs ) 
{
	if (this != &rhs)
	{
		_value = rhs._value;
		_max_body_size = rhs._max_body_size;
		_errors = rhs._errors;
		_methods = rhs._methods;
		_locations = rhs._locations;
	}
	return *this;
}

}
