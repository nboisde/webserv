#include "CGI.hpp"
#include <stdlib.h>
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

namespace ws{


void	CGI::init_conversion( Server const & serv )
{
	typedef std::pair<std::string, std::string>	pair;

	std::map<std::string, std::string>::iterator ite = _header.end();

	_conversion.insert(pair("PHP_SELF", _header["url"]));
	_conversion.insert(pair("GATEWAY_INTERFACE", "CGI/1.1"));
	_conversion.insert(pair("SERVER_ADDR", serv.getIp()));
	_conversion.insert(pair("SERVER_NAME", "webzerv"));
	_conversion.insert(pair("SERVER_PROTOCOL", "HTTP/1.1"));
	//char *timestr;
	//_conversion.insert(pair("REQUEST_TIME", std::string(itoa(time(NULL), timestr, 10))))
	if (_header.find("Method") != ite)
		_conversion.insert(pair("REQUEST_METHOD", _header["Method"]));
	if (_header.find("Content-Length") != ite)
		_conversion.insert(pair("CONTENT_LENGTH", _header["Content-Length"]));
	if (_header.find("Content-Type") != ite)
		_conversion.insert(pair("CONTENT_TYPE", _header["Content-Type"]));
	if (_header.find("Host") != ite)
	{
		_conversion.insert(pair("SERVER_PORT", _header["Host"].substr(_header["Host"].find_last_of(":") + 1)));
		_conversion.insert(pair("SERVER_NAME", _header["Host"].substr(0, _header["Host"].find_last_of(":"))));
	}
	_conversion.insert(pair("QUERY_STRING", ""));
	_conversion.insert(pair("SCRIPT_NAME", _bin_location));
}

CGI::CGI( Client const & cli , Server const & serv) : _bin_location("/usr/bin/php-cgi")
{
	this->_header = cli.getReq().getHead();
	//for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); it++)
	//	std::cout << (*it).first << "->" << (*it).second << std::endl;
	std::cout << std::endl;
	init_conversion( serv );
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
		
		//DEBUG//
		//for (int j = 0; _env[j]; j++)
		//	std::cout << _env[j] << std::endl;
	}
	else
		std::cout << "Other Method" << std::endl;
	return SUCCESS;
}

int		CGI::generate_arg( void ){
	_arg = (char**)malloc(sizeof(char *) * 3);

	std::string tmp = ".";
	tmp += _header["url"];

	_arg[0] = strdup(_bin_location.c_str());
	_arg[1] = strdup(tmp.c_str());
	_arg[2] = NULL;
	return SUCCESS;
}

int		CGI::execute( Client & cli ){
	
	int fd[2];
	pipe(fd);
	int child_stat;
	pid_t pid = fork();
	
	if (pid == 0)
	{
		dup2(fd[1], STDOUT);
		close(fd[0]);
		close(fd[1]);
		execve(_bin_location.c_str(), _arg, NULL);
		//execlp(_bin_location.c_str(), "/usr/bin/php-cgi", "./www/php/bonjour.php", _env);
		exit(SUCCESS);
	}
	waitpid(pid, &child_stat, 0);
	
	//DEBUG//
	std::cout << "Child Return Value = " << WEXITSTATUS(child_stat) << std::endl;
	
	close(fd[1]);
	std::string response;
	createResponse(fd[0], response);
	std::cout << "Child Response content = " << response << std::endl;
	close(fd[0]);
	cli.getRes().setContent(response);
	//cli.getRes().treatCGI(0, response);
	return SUCCESS;
}

int	CGI::createResponse(int fd, std::string & response)
{
	size_t	buff_size = 80;
	char buff[buff_size];

	bzero(buff, buff_size);
	while (read(fd, &buff, buff_size) != 0)
	{
		response += buff;
		bzero(buff, buff_size);
	}
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