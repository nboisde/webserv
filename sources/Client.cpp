#include "Client.hpp"

namespace ws
{

/*
** ------------------------------- CONSTRUCTOR / DESTRUCTOR--------------------
*/ 
Client::Client( void ) {}

Client::Client( int fd, struct sockaddr_in *cli_addr, config_type conf ) : _fd(fd), _status(OK), _config(conf) {
	_ip = inet_ntoa(cli_addr->sin_addr);
	
	std::stringstream port;
	port << ntohs(cli_addr->sin_port);
	_port += port.str();
}

Client::Client( Client const & src ) { *this = src; }
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

#include <sys/types.h>
#include <sys/stat.h>

// struct stat info;

// if( stat( pathname, &info ) != 0 )
//     printf( "cannot access %s\n", pathname );
// else if( info.st_mode & S_IFDIR )  // S_ISDIR() doesn't exist on my windows 
//     printf( "%s is a directory\n", pathname );
// else
//     printf( "%s is no directory\n", pathname );


std::string Client::uploadPath( void )
{
	std::map<std::string, std::string> ml = _config["location"]._locations;
	std::string s = "";
	if (ml.find("upload") == ml.end())
		return s;
	struct stat info;
	if (stat( ml["upload"].c_str(), &info) != 0)
	{
		std::cout << RED << "upload directory dosn't exists" << RESET << std::endl;
		std::cout << GREEN << "File will be registered by default at the root of the server." << RESET << std::endl;
	}
	else if (info.st_mode & S_IFDIR)
	{
		s += ml["upload"];
		s += '/';
	}
	else
	{
		std::cout << RED << "upload location in configuration is not a directory" << RESET << std::endl;
		std::cout << GREEN << "File will be registered by default at the root of the server." << RESET << std::endl;
	}
	std::cout << s << std::endl;
	return s;
}

// SI FORMULAIRE GERE PAR CGI, EDITER _HEAD...
int Client::uploadFiles( void )
{
	std::string data = _req.getBody();
	std::string file;
	while (!data.empty())
	{
		int save = 1;
		int crlf = data.find("\r\n\r\n");
		int f_index = data.find("filename") + 10;
		if (f_index == -1)
			return ERROR;
		std::string f_name = "";
		std::string extension = "";
		while (data[f_index] != '\"')
		{
			f_name += data[f_index];
			f_index++;
		}
		if (f_name.length() == 0)
			save = 0;
		std::string tmp;
		if (crlf != -1)
			tmp = data.substr(crlf + 4, data.length() - (crlf + 4));
		else
			tmp = data;
		int bd = tmp.find(_req.getBoundary());
		if (bd == -1)
			break ;
		else
		{
			while (tmp[bd] && tmp[bd] == '-')
				bd--;
		}
		std::string f_content = tmp.substr(0, bd);
		if (save == 1)
		{
			std::string path = uploadPath();
			std::cout << FIRE << "[" << path << "]" << RESET << std::endl;
			if (!strIsPrintable(f_name))
			{
				int ex = f_name.find(".");
				if (ex == -1)
					extension = f_name.substr(ex, f_name.length() - ex);
				else
					extension = f_name;
				f_name.clear();
				f_name = "file";
				f_name += extension;
			}	
			int fd = -1;
			if (path.length() == 0)
				fd = open(f_name.c_str(), O_CREAT | O_WRONLY, 0644);
			else
			{
				path += f_name;
				fd = open(path.c_str(), O_CREAT | O_WRONLY, 0644);
			}
			if (fd < 0)
				continue ;
			int ret;
			ret = write(fd, f_content.c_str(), f_content.length());
			if (ret == -1)
				std::cout << "Writing problem" << std::endl;
			close(fd);
		}
		int forward = tmp.find(_req.getBoundary()) + _req.getBoundary().length();
		while (tmp[forward] && (tmp[forward] == '-' || tmp[forward] == '\r' || tmp[forward] == '\n'))
			forward++;
		data = tmp.substr(forward, tmp.length() - forward);
	}
	return SUCCESS;
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
		// DISPLAY IN ACCESS LOG
		// std::cout << BLUE;
		// std::cout << "========================= HEADER =========================" << std::endl;
		// std::cout << _req.getHeader();// << std::endl;
		// std::cout << "==========================================================" << std::endl;
		// std::cout << RESET;
		// POSSIBILITE D'IMPLEMENTER UPLOAD
		//int fd = open("w.pdf", O_WRONLY | O_CREAT);
		//write(fd, _req.getBody().c_str(), _req.getBody().length());
		//std::cout << "------------------------- BODY ---------------------------" << std::endl;
		//write(1, _req.getBody().c_str(), _req.getBody().length());
		//std::cout << "----------------------------------------------------------" << std::endl;
		//close(fd);
		//_status = OK;
		bridgeParsingRequest();
		if (_status == OK && _req.getMultipart() == 1)
			uploadFiles();
		return WRITING;
	}
	return READING;
}

void Client::bridgeParsingRequest( void )
{
	int not_all = 1;

	for (std::vector<std::string>::iterator it = _config["method"]._methods.begin(); it != _config["method"]._methods.end(); it++)
	{
		if ((*it) == _req.getHead()["method"])
			not_all = 0;
	}
	if (not_all == 1)
		_status = NOT_ALLOWED;
	else if (static_cast<size_t>(_req.getBody().length()) > _config["client_max_body_size"]._max_body_size
	|| static_cast<size_t>(_req.getContentLength()) > _config["client_max_body_size"]._max_body_size)
		_status = REQUEST_ENTITY_TOO_LARGE;
	else
		_status = OK;
}

int Client::send( void )
{
	int			ret;
	const char* prov;
	std::string str = _res.getResponse();
	int			len = str.size();

	if (len > BUFFER_SIZE)
		len = BUFFER_SIZE;
	prov = str.c_str();
	ret = ::send(_fd, prov, len, 0);
	if (ret < 0)
	{
		perror(" send() failed");
		return (ERROR);
	}
	else if (ret < BUFFER_SIZE)
		return CLOSING;
	_res.setResponse(_res.getResponse().substr(ret));
	return WRITING;
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
	int	attachement = url.find("/download/");

	if (pos < 0 || attachement >= 0)
	{
		std::cout << "URL\t\t" << url << std::endl;
		return ;
	}
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
	std::cout << "PATH " << _file_path << std::endl;
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

	std::cout << "STATUS " << _status << std::endl;
	saveLogs();
	if (_status != OK)
		executeError(serv, port);
	else
	{
		res_type = checkURI(port, _req.getHead()["url"]);
		if (res_type == R_PHP || res_type == R_PY)
			executePhpPython(serv, port, res_type);
		else if (res_type == R_HTML)
			executeHtml(serv, port);
		else if (res_type == R_ERR)
			executeError(serv, port);
	}
	return SUCCESS;
}

int Client::executePhpPython( Server const & serv, Port & port, int extension_type)
{
	CGI cgi(*this, port, serv, extension_type);
	cgi.execute(*this);
	return SUCCESS;
}

int	Client::executeHtml(Server const & serv, Port & port )
{
	std::cout << "HTML EXECUTION" << std::endl;
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
	_res.setContentDisposition(_file_path);
	_res.response(_status);
	return SUCCESS;
}

int	Client::executeError( Server const & serv, Port & port )
{
	std::cout << "ERROR EXECUTION" << std::endl;
	(void)serv;
	std::map<std::string, Value>	config = port.getConfig();
	Value							error = config["error_page"];
	std::string						error_file_path = error._errors[_status];
	std::string						body;

	if (error_file_path.size())
	{
		checkURI(port, error_file_path);
		_status = 301;
		_res.resetResponse();
		std::string loc("Location: ");
		loc += error_file_path;
		_res.setHeader(loc);
		_res.response(_status);
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

void	Client::saveLogs( void )
{
	std::ofstream		ofs("./logs/access.log", std::ios_base::app);
	std::string			content;

	ofs << "CLIENT " << _fd << std::endl;
	ofs << _req.getHeader() << std::endl;
	ofs.close();
}

void Client::closeConnection(){}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int									Client::getStatus( void ) const { return _status; }
int									Client::getFd(void) const { return _fd; }
Request &							Client::getReq( void ) { return _req; }
Request								Client::getReq( void ) const { return _req; }
Response &							Client::getRes( void ) { return _res; }

std::string							Client::getFilePath( void ) const { return _file_path; }
std::string							Client::getIp( void ) const { return _ip; }
std::string							Client::getPort( void ) const { return _port; }
ws::Response						Client::getRes(void ) const { return _res; }
std::map<std::string, ws::Value>	Client::getConfig( void ) const { return _config; }

}