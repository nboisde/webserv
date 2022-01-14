#include "CGI.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

namespace ws{


void	CGI::init_conversion( void )
{
	typedef std::pair<std::string, std::string>	pair;

<<<<<<< HEAD
	std::map<std::string, std::string>::iterator ite = _header.end();

	if (_header.find("Content-Length") != ite)
		_conversion.insert(pair("CONTENT_LENGTH", _header["Content-Length"]));
	if (_header.find("Content-Type") != ite)
		_conversion.insert(pair("CONTENT_TYPE", _header["Content-Type"]));
	if (_header.find("Method") != ite)
		_conversion.insert(pair("REQUEST_METHOD", _header["Method"]));
	if (_header.find("protocol") != ite)
		_conversion.insert(pair("SERVER_PROTOCOL", _header["protocol"]));
	if (_header.find("Host") != ite)
	{
		_conversion.insert(pair("SERVER_PORT", _header["Host"].substr(_header["Host"].find_last_of(":") + 1)));
		_conversion.insert(pair("SERVER_NAME", _header["Host"].substr(0, _header["Host"].find_last_of(":"))));
	}
	_conversion.insert(pair("QUERY_STRING", ""));
	_conversion.insert(pair("GATEWAY_INTERFACE", "CGI/1.1"));
	_conversion.insert(pair("SCRIPT_NAME", _bin_location));
=======
	_conversion.insert(pair("REQUEST_METHOD", _header["Method"]));
	_conversion.insert(pair("CONTENT_LENGTH", _header["Content-Length"]));
	_conversion.insert(pair("CONTENT_TYPE", _header["Content-Type"]));
	_conversion.insert(pair("GATEWAY_INTERFACE", "CGI/1.1"));
	_conversion.insert(pair("QUERY_STRING", ""));
	_conversion.insert(pair("SCRIPT_NAME", _bin_location));
	_conversion.insert(pair("SERVER_PORT", &(_header["Host"].substr(_header["Host"].find_last_of(":"))));

>>>>>>> CGI start
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
		int i = 0;
		this->_env = new char*[_conversion.size() + 1];
		for (std::map<std::string, std::string>::iterator it = _conversion.begin(); it != _conversion.end(); it++)
		{
			std::stringstream new_var;

			new_var << (*it).first << "=" << (*it).second;
			_env[i] = strdup(new_var.str().c_str());
			_env[i][new_var.str().size()] = 0;
			i++;
		}
		_env[_conversion.size()] = 0;
		for (int j = 0; _env[j]; j++)
			std::cout << _env[j] << std::endl;
	}
	else
		std::cout << "Other Method" << std::endl;
	return SUCCESS;
}

int		CGI::generate_arg( void ){
	return SUCCESS;
}

int		CGI::execute( void ){
	pid_t pid = fork();
	int fd[2];

	pipe(fd);
	if (pid == 0)
	{
		dup2(fd[1], STDOUT);
		close(fd[0]);
		close(fd[1]);
		execlp(_bin_location.c_str(), "/usr/bin/php-cgi", "./www/php/bonjour.php", _env);
		exit(SUCCESS);
	}
	char buf[1];
	waitpid(pid, 0, 0);
	//int fd2 = open("CGI_ret", O_WRONLY | O_CREAT | O_TRUNC);
	//dup2(fd[0], fd2);
	while (read(fd[0], buf, 1))
		std::cout << *buf;
	std::cout << std::endl;
 	close(fd[0]);
	close(fd[1]);
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