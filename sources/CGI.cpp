#include "CGI.hpp"
#include <stdlib.h>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

namespace ws{


CGI::CGI( Client const & cli , Port const & port, Server const & serv, int extension_type) : _arg(NULL), _env(NULL)
{
	if (extension_type == R_PY)
	{	
		_extension = ".py";
		_bin_location = "/usr/bin/python3";
	}
	else
	{
		_extension = ".php";
		_bin_location = "/usr/bin/php-cgi";
	}
	this->_header = cli.getReq().getHead();
	init_conversion( cli, port, serv );
	generate_env();
	generate_arg(cli);
}

void	CGI::init_conversion( Client const & cli, Port const & port, Server const & serv )
{
	typedef std::pair<std::string, std::string>	pair;
	typedef std::map<std::string, std::string>::iterator map_iterator;
	
	map_iterator ite = _header.end();

	if (_header.find("content-length") != ite)
	{
		_conversion.insert(pair("CONTENT_LENGTH", _header["content-length"]));
		_conversion.insert(pair("HTTP_CONTENT_LENGTH", _header["content-length"]));
	}
	if (_header.find("content-type") != ite)
	{
		_conversion.insert(pair("CONTENT_TYPE", _header["content-type"]));
		_conversion.insert(pair("HTTP_CONTENT_TYPE", _header["content-type"]));
	}
	_conversion.insert(pair("SERVER_PROTOCOL", "HTTP/1.1"));
	_conversion.insert(pair("GATEWAY_INTERFACE", "CGI/1.1"));
	_conversion.insert(pair("SERVER_SOFTWARE", "webzerver/0.9"));
	_conversion.insert(pair("REDIRECT_STATUS", "200"));

	_conversion.insert(pair("REMOTE_ADDR", cli.getIp()));
	_conversion.insert(pair("REMOTE_PORT", cli.getPort()));
	_conversion.insert(pair("SERVER_ADDR", serv.getIp()));
	std::stringstream server_port;
	server_port << ntohs(port.getPortAddr().sin_port);
	_conversion.insert(pair("SERVER_PORT", server_port.str()));
	
	std::string url = _header["url"];
	std::string root = cli.getFilePath().substr(0, cli.getFilePath().find(url));
	_conversion.insert(pair("DOCUMENT_ROOT", root));
	_conversion.insert(pair("DOCUMENT_URI", url.substr(0, url.find(_extension) + _extension.size())));
	_conversion.insert(pair("SCRIPT_NAME", url.substr(0, url.find(_extension) + _extension.size())));
	_conversion.insert(pair("PHP_SELF", url.substr(0, url.find(_extension) + _extension.size())));	
	_conversion.insert(pair("REQUEST_URI", url));
	
	std::string query;
	if (url.find(".php?") != std::string::npos)
		query = url.substr(url.find(".php?") + 4);
	_conversion.insert(pair("QUERY_STRING", query));

	_conversion.insert(pair("SCRIPT_FILENAME", cli.getFilePath()));
	if (_header.find("method") != ite)
		_conversion.insert(pair("REQUEST_METHOD", _header["method"]));

	_conversion.insert(pair("HTTPS", ""));
	if (_header.find("accept") != ite)
		_conversion.insert(pair("HTTP_ACCEPT", _header["accept"]));
	if (_header.find("accept-charset") != ite)
		_conversion.insert(pair("HTTP_ACCEPT_CHARSET", _header["accept-charset"]));
	if (_header.find("accept-encoding") != ite)
		_conversion.insert(pair("HTTP_ACCEPT_ENCODING", _header["accept-encoding"]));
	if (_header.find("accept-language") != ite)
		_conversion.insert(pair("HTTP_ACCEPT_LANGUAGE", _header["accept-language"]));
	if (_header.find("connection") != ite)
		_conversion.insert(pair("HTTP_CONNECTION", _header["connection"]));
	if (_header.find("host") != ite)
		_conversion.insert(pair("HTTP_HOST", _header["host"]));
	if (_header.find("user-agent") != ite)
		_conversion.insert(pair("HTTP_USER_AGENT", _header["user-agent"]));
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
	if (_arg)
	{
		for (int i = 0; _arg[i] != NULL; i++)
			free(_arg[i]);
		delete[](_arg);
	}
	if (_env)
	{
		for (int i = 0; _env[i] != NULL; i++)
				free(_env[i]);
		delete[](_env);
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
	if (_header["method"] == "GET" | _header["method"] == "POST")
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
	}
	else
		std::cout << "Other Method" << std::endl;
	return SUCCESS;
}

int		CGI::generate_arg( Client const & cli ){
	std::string file_path = cli.getFilePath();
	
	file_path = file_path.substr(0, file_path.find_last_of(_extension) + _extension.size());
	_arg = new char*[3];
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
<<<<<<< HEAD
		if (_header["Method"] == "POST")
=======
		//for(int i = 0; _env[i]; i++)
		//	std::cerr << _env[i] << std::endl;
		if (_header["method"] == "POST")
>>>>>>> CGI environement in full lower case
		{
			std::string body = cli.getReq().getBody();
			char buf[BUFFER_SIZE];
			memset(&buf, 0, BUFFER_SIZE);
			int fd2[2];
			pipe(fd2);
			while (!body.empty())
			{
				size_t added = body.copy(buf, BUFFER_SIZE);
				size_t ret = write(fd2[1], buf, added);
				body = body.substr(ret);
				memset(&buf, 0, added);
			}
			memset(&buf, 0, BUFFER_SIZE);
			close(fd2[1]);
			dup2(fd2[0], STDIN);
			close(fd2[0]);
		}
		dup2(fd[1], STDOUT);
		close(fd[0]);
		close(fd[1]);
		execve(_bin_location.c_str(), _arg, _env);
		exit(SUCCESS);
	}
	waitpid(pid, &child_stat, 0);
	close(fd[1]);
	std::string response = concatenateResponse(fd[0]);
	cli.getRes().treatCGI(response);
	cli.getRes().response(CGI_FLAG);
	//std::cout << cli.getRes().getResponse() << std::endl;
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
		if (line)
			free(line);
		line = NULL;
	}
	if (line)
		free(line);
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
