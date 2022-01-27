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
		std::cout << RED << "400 bad request (Header reception)" << RESET << std::endl;
		_status = BAD_REQUEST;
		return WRITING;
	}
	if (req == SUCCESS)
	{
		int head_err = _req.fillHeaderAndBody();
		if (head_err == ERROR)
		{
			std::cout << RED << "400 bad request (Header reception)" << RESET << std::endl;
			_status = BAD_REQUEST;
			return WRITING;
		}
		std::cout << BLUE;
		std::cout << _req.getHeader() << std::endl;
		std::cout << RESET;
		write(1, _req.getBody().c_str(), _req.getBody().length());
		_status = OK;
		return WRITING;
	}
	return READING;
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
	std::cout << "EXTENSION " << extension << std::endl;
	std::map<std::string, Value> config = port.getConfig();
	Value location = config["location"];
	std::string	path = location._locations[extension];
	std::cout << "PATH " << path << std::endl;
	if (path.size())
		url = path + url;
	std::cout << "YRL " << url << std::endl;
}

int	Client::checkCGI( std::string & url )
{	
	int		query_pos = url.find("?");
	int		size = url.size();
	int		php_pos = url.find(".php");

	if (query_pos >= 0 )
			url = url.substr(0, query_pos);
	if (php_pos >= 0 && size - php_pos == 4)
		return (R_CGI);
	else if (query_pos >= 0 && php_pos >= 0 && php_pos < query_pos)
		return (R_CGI);
	else
		return (R_HTML);
}

int	Client::checkURI( Port & port )
{
	std::string			root;
	std::string			url;	
	char				*buf = NULL;
	std::stringstream	file_path;
	int					ret;

	url =_req.getHead()["url"];
	if (url == "/")
		url = port.getConfig()["index"]._value;
	root = port.getConfig()["root"]._value;
	buf = getcwd(buf, 0);
	ret = checkCGI(url);
	checkPath(url, port);
	checkExtension(url, port);
	file_path << buf << root << url;
	_file_path = file_path.str();
	int fd = ::open(_file_path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		_status = NOT_FOUND;
		return ERROR;
	}
	close(fd);
	return (ret);
}

int	Client::executeCGI( Server const & serv, Port & port )
{
	int	res_type;

	res_type = checkURI(port);
//	std::cout << "Response " << res_type << std::endl;
	if (res_type == R_CGI)
	{
		CGI cgi(*this, port, serv);
		cgi.execute(*this);
	}
	else if (res_type == R_HTML)
		executeHtml( port );
	else
		_res.response(_status);
	return SUCCESS;
}

int	Client::executeHtml(Port & port )
{
	(void)port;
	std::ifstream	ifs(_file_path.c_str());
	std::string		buffer;
	std::string		content;

 	while (getline(ifs, buffer))
		content += buffer;
	ifs.close();
	_res.setBody(content);
	_res.setContentType(_file_path);
	_res.response(_status);
	std::cout << _res.getResponse() << std::endl;
	return SUCCESS;
}

void Client::closeConnection(){}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int			Client::getStatus( void ) const { return _status; }
int			Client::getFd(void) const { return _fd; }
Request &	Client::getReq( void ) { return _req; }
Request		Client::getReq( void ) const { return _req; }
Response &	Client::getRes( void ) { return _res; }

std::string	Client::getFilePath( void ) const{
	return (_file_path);
}

std::string		Client::getIp( void ) const{
	return (_ip);
}

std::string		Client::getPort( void ) const{
	return (_port);
}

ws::Response		Client::getRes(void ) const{
	return (_res);
}

}
