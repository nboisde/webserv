#include "Client.hpp"

namespace ws
{

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/ 
Client::Client( void ) {}
Client::Client( int fd ) : _fd(fd), _status(OK) {}
Client::Client( Client const & src ) { *this = src; }

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Client::~Client() {}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Client &				Client::operator=( Client const & rhs )
{
	if ( this != &rhs )
	{
		this->_fd = rhs.getFd();
		this->_status = rhs.getStatus();
		this->_req = rhs._req;
		this->_res = rhs._res;
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
	//int			len;
	const char* prov;
	std::string str = _res.response(_status);

	//len = str.size();
	prov = str.c_str();
	ret = ::send(_fd, prov, str.size(), 0);
	if (ret < 0)
	{
		perror(" send() failed");
		return (ERROR);		
	}

	// DEUX CAS A GERER SUR CETTE FONCTION supposement. (avec content-length ou transfer encoding.)
	/*
	if (len > BUFFER_SIZE)
		len = BUFFER_SIZE;
	//std::cout << "SEND FD = " << _fd << std::endl;
	ret = ::send(_fd, prov, len, 0);
	//std::cout << "RET IN SENDING " << ret << std::endl;
	if (ret < 0)
	{
		perror(" send() failed");
		return (ERROR);		
	}
	//str.erase(0, ret - 1);
	if (str.empty())
		return CLOSING;
	//return (WRITING);
	return (SUCCESS);*/
	return CLOSING;
}

void	Client::checkPath( std::string & url, Port & port )
{
	std::map<std::string, Value> config = port.getConfig();
	Value location = config["location"];
	std::string	path = location._locations[url];
	if (path.size())
		url = path + url;
}
void	Client::checkExtension( std::string & url, Port & port )
{
	std::string extension = url.substr(url.find("."));
	std::map<std::string, Value> config = port.getConfig();
	Value location = config["location"];
	std::string	path = location._locations[extension];
	if (path.size())
		url = path + url;
}

int	Client::checkURI( Port & port )
{
	std::string	url;
	std::string	root;
	char		*buf = NULL;
	size_t		pos;
	size_t		size;
	std::stringstream file_path;

	url =_req.getHead()["url"];
	pos = url.find("?");
	size = url.size();
	if (pos >= 0 && pos < size)
		url = url.substr(0, pos);
	if (url == "/")
		url = port.getConfig()["index"]._value;
	checkPath(url, port);
	checkExtension(url, port);
	root = port.getConfig()["root"]._value;
	buf = getcwd(buf, 0);
	file_path << buf << root << url;
	_file_path = file_path.str();
	int fd = ::open(_file_path.c_str(), O_RDONLY);
	if (fd < 0)
		return ERROR;
	close(fd);
	pos = _file_path.find(".php");
	size = _file_path.size();
	if (size - pos == 4)
		return (R_CGI);
	return (R_HTML);
}

int	Client::executeCGI( Server const & serv, Port & port )
{
	int	res_type;
	CGI cgi(*this, serv);

	res_type = checkURI(port);
	std::cout << "Response " << res_type << std::endl;
	if (res_type == R_CGI)
		cgi.execute(*this);
	else if (res_type == R_HTML)
		executeHtml( port );
	else 
		return ERROR;
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

}
