#include "CGI.hpp"
#include <stdlib.h>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

namespace ws{

struct	url_parser
{
	std::string script_name;
	std::string	document_root;
	std::string	path_info;
	std::string	script_filename;
	std::string	request_uri;
	std::string	document_uri;
};

CGI::CGI( Client const & cli , Port const & port, Server const & serv) : _bin_location("/usr/bin/php-cgi")
{
	this->_header = cli.getReq().getHead();
	init_conversion( cli, port, serv );
	generate_env();
	generate_arg(cli);
}

void	CGI::init_conversion( Client const & cli, Port const & port, Server const & serv )
{
	typedef std::pair<std::string, std::string>	pair;

	std::map<std::string, std::string>::iterator ite = _header.end();

	if (_header.find("Content-Length") != ite)
		_conversion.insert(pair("CONTENT_LENGTH", _header["Content-Length"]));
	if (_header.find("Content-Type") != ite)
		_conversion.insert(pair("CONTENT_TYPE", _header["Content-Type"]));
	_conversion.insert(pair("SERVER_PROTOCOL", "HTTP/1.1"));

	_conversion.insert(pair("REMOTE_ADDR", cli.getIp()));
	_conversion.insert(pair("REMOTE_PORT", cli.getPort()));
	_conversion.insert(pair("SERVER_ADDR", serv.getIp()));
	std::stringstream server_port;
	server_port << ntohs(port.getPortAddr().sin_port);
	_conversion.insert(pair("SERVER_PORT", server_port.str()));
	
	_conversion.insert(pair("REDIRECT_STATUS", "200"));

	struct url_parser	my_url;
	
	//CAUSE PB
	//_conversion.insert(pair("GATEWAY_INTERFACE", "CGI/1.1"));
	//if (_header.find("Method") != ite)
	//	_conversion.insert(pair("REQUEST_METHOD", _header["Method"]));
	//_conversion.insert(pair("SERVER_SOFTWARE", "webzerver/0.9"));
	//

	//URL MANAGEMENT//
	std::string url(_header["url"]);
	std::cout << "URL BRUT = " << url << std::endl;
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
			free(_arg[i]);
		free(_arg);
	}
	if (!_env)
	{
		for (int i = 0; _env[i] != NULL; i++)
				free(_env[i]);
		free(_env);
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
		this->_arg = rhs.getArg();
		this->_env = rhs.getEnv();
		this->_conversion = rhs.getConversion();
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

int		CGI::generate_arg( Client const & cli ){
	std::string file_path = cli.getFilePath();
	
	file_path = file_path.substr(0, file_path.find_last_of(".php") + 4);
	_arg = (char**)malloc(sizeof(char *) * 3);

	_arg[0] = strdup(_bin_location.c_str());
	_arg[1] = strdup(file_path.c_str());
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
		execve(_bin_location.c_str(), _arg, _env);
		exit(SUCCESS);
	}
	waitpid(pid, &child_stat, 0);
	
	//DEBUG//
	//std::cout << "Child Return Value = " << WEXITSTATUS(child_stat) << std::endl;
	
	close(fd[1]);
	std::string response = concatenateResponse(fd[0]);
	cli.getRes().treatCGI(response);
	cli.getRes().response();
	return SUCCESS;
}

std::string	CGI::concatenateResponse(int fd)
{
	std::string response;
	char *line = NULL;
	size_t line_size = 0;
	FILE * pipe_end = fdopen(fd, "r");

	while (getline(&line, &line_size, pipe_end) != -1)
	{
		response += line;
		free(line);
		line = NULL;
	}
	fclose(pipe_end);
	return response;
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

std::map<std::string, std::string>	CGI::getConversion( void ) const{
	return (_conversion);
}

char **		CGI::getArg( void ) const{
	return (_arg);
}

char **		CGI::getEnv( void ) const {
	return (_env);
}

}

/* ************************************************************************** */