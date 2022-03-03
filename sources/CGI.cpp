#include "CGI.hpp"
#include <stdlib.h>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

namespace ws{


CGI::CGI( Client const & cli , Port const & port, Server const & serv) : _arg(NULL), _env(NULL)
{
	std::string bin_path = cli.getConfig()[cli.getHostname()]["cgi"]._list[cli.getExtension()];
	_extension = cli.getExtension();
	_bin_location = bin_path;
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
		_conversion.insert(pair("CONTENT_LENGTH", _header["content-length"]));
	if (_header.find("content-type") != ite)
		_conversion.insert(pair("CONTENT_TYPE", _header["content-type"]));
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
	std::string root("");
	root += port.getConfig()[cli.getHostname()]["root"]._value;
	std::string uri_query = _header["url"];

	std::string url = cli.getFilePath();
	size_t		pos = url.find(root) + root.size();

	_conversion.insert(pair("DOCUMENT_ROOT", root));
	_conversion.insert(pair("DOCUMENT_URI", url.substr(pos, url.find(_extension) + _extension.size() - pos)));

	_conversion.insert(pair("SCRIPT_NAME", url.substr(pos, url.find(_extension) + _extension.size() - pos)));
	_conversion.insert(pair("PHP_SELF", url.substr(pos, url.find(_extension) + _extension.size() - pos)));	
	_conversion.insert(pair("REQUEST_URI", uri_query));
	std::string query;
	if (uri_query.find(".php?") != std::string::npos)
		query = uri_query.substr(uri_query.find(".php?") + 5);
	_conversion.insert(pair("QUERY_STRING", query));

	_conversion.insert(pair("SCRIPT_FILENAME", cli.getFilePath()));
	if (_header.find("method") != ite)
		_conversion.insert(pair("REQUEST_METHOD", _header["method"]));

	_conversion.insert(pair("HTTPS", ""));
	for (map_iterator it = _header.begin(); it != ite; it++)
	{
		std::string tmp ("HTTP_");
		tmp += it->first;
		for (int i = 0; tmp[i]; i++)
		{
			tmp[i] = toupper(tmp[i]);
			if (tmp[i] == '-')
				tmp[i] = '_';
		}
		_conversion.insert(pair(tmp, it->second));
	}
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

	int child_stat;

	FILE *tmp = tmpfile();
	if (!tmp)
		return ERROR;
	pid_t pid = fork();
	if (pid < 0)
		return ERROR;
	if (pid == 0)
	{
		if (_header["method"] == "POST")
		{	
			int ret = dup2(fileno(cli.getTmpFile()), STDIN);
			if (ret < 0)
				exit(ERROR);
		}
		int ret = dup2(fileno(tmp), STDOUT);
		if (ret < 0)
			exit(ERROR);
		if (execve(_bin_location.c_str(), _arg, _env) < 0)
			exit(ERROR);
		exit(SUCCESS);
	}
	waitpid(pid, &child_stat, 0);
	int ret = WEXITSTATUS(child_stat);
	if (ret == ERROR)
		return ret;
	rewind(tmp);
	cli.setCGIFile(tmp);
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
