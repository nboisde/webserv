#include "Client.hpp"

namespace ws
{

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/ 
Client::Client( void ) {}

Client::Client( int fd, struct sockaddr_in *cli_addr, config_type conf ) : _fd(fd), _status(OK), _config(conf) {
	_ip = inet_ntoa(cli_addr->sin_addr);
	
	std::stringstream port;
	port << ntohs(cli_addr->sin_port);
	_port += port.str();
}

Client::Client( Client const & src ) 
{
	*this = src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Client::~Client() {}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Client &	Client::operator=( Client const & rhs )
{
	if ( this != &rhs )
	{
		this->_fd = rhs.getFd();
		this->_status = rhs.getStatus();
		this->_req = rhs.getReq();
		this->_res = rhs.getRes();
		this->_file_path = rhs.getFilePath();
		this->_ip = rhs.getIp();
		this->_port = rhs.getPort();
		this->_config = rhs.getConfig();
	}
	return *this;
}

int Client::receive(void)
{
	char	buffer[BUFFER_SIZE];

	for (size_t i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = 0;
	int ret = recv(_fd, buffer, BUFFER_SIZE - 1, 0);
	if ( ret < 0 )
	{
		perror("\nIn recv");
		_status = INTERNAL_SERVER_ERROR;
		return WRITING;
	}
	std::string tmp(buffer, ret);
	int req = _req.concatenateRequest(tmp);
	if (req == -1)
	{
		std::cout << RED << "400 bad request (Header reception 1)" << RESET << std::endl;
		_status = _req.getStatus();
		return WRITING;
	}
	if (req == SUCCESS)
	{
		int head_err = _req.fillHeaderAndBody();
		if (head_err == ERROR)
		{
			std::cout << RED << "400 bad request (Header reception 2)" << RESET << std::endl;
			//_status = BAD_REQUEST;
			_status = _req.getStatus();
			return WRITING;
		}
		std::cout << BLUE;
		std::cout << _req.getHeader() << std::endl;
		std::cout << RESET;
		write(1, _req.getBody().c_str(), _req.getBody().length());
		_status = OK;
		bridgeParsingRequest();
		return WRITING;
	}
	return READING;
}

void Client::bridgeParsingRequest( void )
{
	int not_all = 1;
	//int max_size = 0;

	std::cout << "=================== DEV ==================" << std::endl;
	std::cout << "MAX BODY SIZE : " << _config["client_max_body_size"]._max_body_size << std::endl;
	std::cout << (*(_config.find("client_max_body_size"))).second._max_body_size << std::endl;
	std::cout << "Methods allowed : ";
	for (std::vector<std::string>::iterator it = _config["method"]._methods.begin(); it != _config["method"]._methods.end(); it++)
		std::cout << (*it) << ", ";
	std::cout << std::endl;
	std::cout << "==========================================" << std::endl;
	std::cout << _req.getHead()["Method"] << std::endl;
	for (std::vector<std::string>::iterator it = _config["method"]._methods.begin(); it != _config["method"]._methods.end(); it++)
	{
		if ((*it) == _req.getHead()["Method"])
			not_all = 0;
	}
	if (not_all == 1)
		_status = NOT_ALLOWED;
	else if (static_cast<size_t>(_req.getBody().length()) > _config["client_max_body_size"]._max_body_size
	|| static_cast<size_t>(_req.getContentLength()) > _config["client_max_body_size"]._max_body_size)
		_status = REQUEST_ENTITY_TOO_LARGE;
}

int Client::send( void )
{
	int			ret;
	const char* prov;
	std::string str = _res.getResponse();

	prov = str.c_str();
	ret = ::send(_fd, prov, str.size(), 0);
	if (ret < 0)
	{
		perror(" send() failed");
		return (ERROR);
	}
	return CLOSING;
}

void	Client::checkPath( std::string & url, Port & port )
{
	std::map<std::string, Value>	config = port.getConfig();
	Value							location = config["location"];
	std::string	path =				location._locations[url];

	if (path.size())
		url = path + url;
}
void	Client::checkExtension( std::string & url, Port & port )
{
	int	pos = url.find(".");
	if (pos < 0)
		return ;
	std::string	extension = url.substr(url.find("."));
	std::cout << std::endl << "EXTENSION\t" << extension << std::endl;
	std::map<std::string, Value> config = port.getConfig();
	Value location = config["location"];
	std::string	path = location._locations[extension];
	std::cout << "PATH\t\t" << path << std::endl;
	if (path.size())
		url = path + url;
	std::cout << "URL\t\t" << url << std::endl;
}

int	Client::checkCGI( std::string & url )
{
	int		query_pos = url.find("?");
	int		php_pos = url.find(".php");
	int		py_pos = url.find(".py");
	int		size = url.size();

	if (query_pos >= 0 )
			url = url.substr(0, query_pos);
	if (php_pos >= 0 && size - php_pos == 4)
		return (R_PHP);
	else if (query_pos >= 0 && php_pos >= 0 && php_pos < query_pos)
		return (R_PHP);
	else if (py_pos >= 0 && size - py_pos == 3)
		return (R_PY);
	else if (py_pos >= 0 && py_pos >= 0 && py_pos < query_pos)
		return (R_PY);
	else
		return (R_HTML);
}

int	Client::checkURI( Port & port, std::string url)
{
	int					ret;
	char				*buf = NULL;
	std::string			root;
	std::stringstream	file_path;

	if (url == "/")
		url = port.getConfig()["index"]._value;
	root = port.getConfig()["root"]._value;
	buf = getcwd(buf, 0);
	ret = checkCGI(url);
	checkPath(url, port);
	checkExtension(url, port);
	file_path << buf << root << url;
	if (buf)
		free(buf);
	_file_path = file_path.str();
	int fd = ::open(_file_path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		_status = NOT_FOUND;
		return R_ERR;
	}
	close(fd);
	return (ret);
}

int	Client::execution( Server const & serv, Port & port )
{
	int	res_type = ERROR;

	if (_status != OK)
		executeError(serv, port);
	else
	{
		res_type = checkURI(port, _req.getHead()["url"]);
		if (res_type == R_PHP)
			executePhp(serv, port);
		else if (res_type == R_PY)
			executePy(serv, port);
		else if (res_type == R_HTML)
			executeHtml(serv, port);
		else if (res_type == R_ERR)
			executeError(serv, port);
	}
	return SUCCESS;
}

int	Client::executePy( Server const & serv, Port & port )
{
	(void)serv;
	(void)port;
	return  SUCCESS;
}

int Client::executePhp( Server const & serv, Port & port )
{
	CGI cgi(*this, port, serv);
	cgi.execute(*this);
	return SUCCESS;
}

int	Client::executeHtml(Server const & serv, Port & port )
{
	(void)port;
	(void)serv;
	std::string		content;
	char *line = NULL;
	size_t n = 0;
	FILE * file = fopen(_file_path.c_str(), "r");
	ssize_t ret = 0;

	while ((ret = getline(&line, &n, file)) != -1)
	{
		content.append(line, ret);
		free(line);
		line = NULL;
	}
	fclose(file);
	_res.setBody(content);
	_res.setContentType(_file_path);
	_res.response(_status);
	std::cout << _res.getResponse() << std::endl;
	return SUCCESS;
}

int	Client::executeError( Server const & serv, Port & port )
{
	(void)serv;
	std::map<std::string, Value>	config = port.getConfig();
	Value							error = config["error_page"];
	std::string						error_file_path = error._errors[_status];
	std::string						body;

	std::cout << "ERROR FILE PATH: [" << error_file_path << std::endl;
	if (error_file_path.size())
	{
		checkURI(port, error_file_path);
		executeHtml(serv, port);
	}
	else
	{
		std::string body = _res.genBody(_status);
		_res.setBody(body);
		_res.setContentType(_file_path);
		_res.response(_status);
	}
	return SUCCESS;
}

void Client::closeConnection(){}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int										Client::getStatus( void ) const { return _status; }
int										Client::getFd(void) const { return _fd; }
Request &								Client::getReq( void ) { return _req; }
Request									Client::getReq( void ) const { return _req; }
Response &								Client::getRes( void ) { return _res; }

std::string								Client::getFilePath( void ) const { return _file_path; }
std::string								Client::getIp( void ) const { return _ip; }
std::string								Client::getPort( void ) const { return _port; }
ws::Response							Client::getRes(void ) const { return _res; }
std::map<std::string, ws::Value>		Client::getConfig( void ) const { return _config; }

}