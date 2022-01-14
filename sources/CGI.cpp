#include "CGI.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

namespace ws{


void	CGI::init_conversion( void )
{
	typedef std::pair<std::string, std::string>	pair;

	_conversion.insert(pair("REQUEST_METHOD", _header["Method"]));
	_conversion.insert(pair("CONTENT_LENGTH", _header["Content-Length"]));
	_conversion.insert(pair("CONTENT_TYPE", _header["Content-Type"]));
	_conversion.insert(pair("GATEWAY_INTERFACE", "CGI/1.1"));
	_conversion.insert(pair("QUERY_STRING", ""));
	_conversion.insert(pair("SCRIPT_NAME", _bin_location));
	_conversion.insert(pair("SERVER_PORT", &(_header["Host"].substr(_header["Host"].find_last_of(":"))));

}

CGI::CGI( Client const & cli ) : _bin_location("/usr/bin/php-cgi")
{
	this->_header = cli.getReq().getHead();
	for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); it++)
		std::cout << (*it).first << "->" << (*it).second << std::endl;
	std::cout << std::endl;
	init_conversion();
	generate_env();
	generate_arg();
}

CGI::CGI( void )
{
	return;
}

CGI::CGI( const CGI & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

CGI::~CGI( void )
{
	if (!_arg)
	{
		for (int i = 0; _arg[i] != NULL; i++)
			delete(_arg[i]);
		delete(_arg);
	}
	if (!_env)
	{
		for (int i = 0; _env[i] != NULL; i++)
				delete(_env[i]);
		delete(_env);
	}
	return;
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

CGI &				CGI::operator=( CGI const & rhs )
{
	if ( this != &rhs )
	{
		this->_bin_location = rhs.getBinLocation();
		this->_header = rhs.getHeader();
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, CGI const & i )
{
	(void) i;
	//o << "Value = " << i.getValue();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/


int		CGI::generate_env( void )
{
	if (_header["Method"] == "GET")
	{
		this->_env = new char*[10];
		std::cout << "HELLO" << std::endl;
	}
	else
		std::cout << "Other Method" << std::endl;
	return SUCCESS;
}

int		CGI::generate_arg( void ){
	return SUCCESS;
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string	CGI::getBinLocation( void ) const{
	return (_bin_location);
}
std::map<std::string, std::string> CGI::getHeader( void ) const{
	return (_header);
}

}

/* ************************************************************************** */