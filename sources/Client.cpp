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

int	Client::checkPath( char * cwd, std::string & root, std::string & url, Port & port )
{
	std::map<std::string, Value>	config = port.getConfig();
	Value							location = config["location"];
	std::string						path = location._locations[url];
	std::stringstream				file_path;
	
	file_path << cwd << root << url;
	if (openFile(file_path.str()) > 0)
	{
		_file_path = file_path.str();
		if (cwd)
			free(cwd);
		return (SUCCESS);
	}
	if (path.size())
	{
		file_path.str("");
		url = path + url;
		file_path << cwd << root << url;
		if (openFile(file_path.str()) > 0)
		{
			_file_path = file_path.str();
			if (cwd)
				free(cwd);
			return (SUCCESS);
		}
	}
	return (ERROR);
}

int	Client::checkExtension( char * cwd, std::string & root, std::string & url, Port & port )
{
	int								pos = url.find(".");
	int								attachement = url.find("/download/");
	std::map<std::string, Value>	config = port.getConfig();
	std::stringstream				file_path;

	if (pos > 0 && attachement < 0)
	{
		Value		location = config["location"];
		std::string	extension = url.substr(pos);
		std::string	path = location._locations[extension];
		if (path.size())
			url = path + url;
	}
	file_path << cwd << root << url;
	if (openFile(file_path.str()) > 0)
	{
		_file_path = file_path.str();
		if (cwd)
			free(cwd);
		return (SUCCESS);
	}
	if (cwd)
		free(cwd);
	return (ERROR);
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

int	Client::openFile( std::string path )
{
	int fd = ::open(path.c_str(), O_RDONLY);
	if (fd < 0)
		return ERROR;
	close(fd);
	fd = ::open(path.c_str(), O_DIRECTORY);
	if 
	(fd > 0)
	{
		close(fd);
		return (ERROR);
	}
	return SUCCESS;
}

int	Client::checkURI( Port & port, std::string url)
{
	int					ret;
	char				*cwd = NULL;
	std::string			root;

	if (url == "/")
		url = port.getConfig()["index"]._value;
	root = port.getConfig()["root"]._value;
	cwd = getcwd(cwd, 0);
	ret = checkCGI(url);
	if (checkPath(cwd, root, url, port) > 0)
		return (ret);
	if (checkExtension(cwd, root, url, port) > 0)
		return (ret);
	_status = NOT_FOUND;
	return (R_ERR);
}

int	Client::execution( Server const & serv, Port & port )
{
	int	res_type = ERROR;

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
		if (line)
			free(line);
		line = NULL;
	}
	if (line)
		free(line);
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
	std::cout << "STATUS " << _status << std::endl;
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
		_res.setContentType("");
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