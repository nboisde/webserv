#include "Client.hpp"

namespace ws
{

typedef	std::map<std::string, Value>	config_type;
typedef std::map<int, std::string>		error_type;
typedef std::map<std::string, config_type>	map_configs;

/*
** ------------------------------- CONSTRUCTOR / DESTRUCTOR--------------------
*/ 
Client::Client( void ) {}

Client::Client( int fd, struct sockaddr_in *cli_addr, map_configs conf ) : _fd(fd), _status(OK), _config(conf), _hostname(LOCALHOST) {
	_ip = inet_ntoa(cli_addr->sin_addr);
	
	std::stringstream port;
	port << ntohs(cli_addr->sin_port);
	_port += port.str();
	_req.setUploadAuthorized(this->uploadAuthorized());
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
		this->_hostname = rhs._hostname;
	}
	return *this;
}

#include <sys/types.h>
#include <sys/stat.h>

std::string Client::uploadPath( void )
{
	std::map<std::string, std::string> ml = _config[_hostname]["location"]._locations;
	std::string s = "";
	
	return (ml["upload"]);
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
			if (path.length() != 0)
				f_name = path + f_name;
			std::fstream fs;
			fs.open(f_name.c_str(), std::fstream::out);
			fs << f_content;
			fs.close();
			chmod(f_name.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		}
		int forward = tmp.find(_req.getBoundary()) + _req.getBoundary().length();
		while (tmp[forward] && (tmp[forward] == '-' || tmp[forward] == '\r' || tmp[forward] == '\n'))
			forward++;
		data = tmp.substr(forward, tmp.length() - forward);
	}
	return SUCCESS;
}

void Client::bridgeParsingRequest( void )
{
	int not_all = 1;

	for (std::vector<std::string>::iterator it = _config[_hostname]["method"]._methods.begin(); it != _config[_hostname]["method"]._methods.end(); it++)
	{
		if ((*it) == _req.getHead()["method"])
			not_all = 0;
	}
	if (not_all == 1)
		_status = NOT_ALLOWED;
	else if (static_cast<size_t>(_req.getBody().length()) > _config[_hostname]["client_max_body_size"]._max_body_size
	|| static_cast<size_t>(_req.getContentLength()) > _config[_hostname]["client_max_body_size"]._max_body_size)
		_status = REQUEST_ENTITY_TOO_LARGE;
	else
		_status = OK;
}

int Client::uploadAuthorized( void )
{
	for (std::vector<std::string>::iterator it = _config[_hostname]["method"]._methods.begin(); it != _config[_hostname]["method"]._methods.end(); it++)
	{
		if ((*it) == "POST")
			return 1;
	}
	return 0;
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
	_req.setUploadAuthorized(this->uploadAuthorized());
	std::string tmp(buffer, ret);
	//std::cout << DEV << "Authorized upload: " << _req.getUploadAuthorized() << RESET << std::endl;
	int req = _req.concatenateRequest(tmp);
	//std::cout << _req.getRawContent() << std::endl;
	if (req == -1 && _req.findContinue() == 0)
	{
		std::cout << RED << "400 bad request (Header reception 1)" << RESET << std::endl;
		_status = _req.getStatus();
		return WRITING;
	}
	if (req == SUCCESS)// && _req.getContinue() == 0)
	{
		int head_err = _req.fillHeaderAndBody();
		std::cout << DEV << "FUCK" << RESET << std::endl;
		std::string tmp2 = _req.getHead()["host"];
		std::cout << tmp2 << std::endl;
		size_t pos = tmp2.find(":");
		if (pos >= static_cast<size_t>(0))
			_hostname = tmp2.substr(pos);
		else
			_hostname = tmp2;
		//std::cout << BLUE << _req.getHeader() << RESET << std::endl;
		//std::cout << DEV << _req.getBody() << RESET << std::endl;
		std::cout << DEV << _req.getRawContent() << RESET << std::endl;
		_req.setContinue(0);
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

int	Client::checkPath( std::string & root, std::string & url )
{
	Value							location = _config[_hostname]["location"];
	std::string						path = location._locations[url];
	std::stringstream				file_path;
	
	file_path << root << url;
	if (openFile(file_path.str()) > 0)
	{
		_file_path = file_path.str();
		return (SUCCESS);
	}
	if (path.size())
	{
		file_path.str("");
		url = path + url;
		file_path << root << url;
		if (openFile(file_path.str()) > 0)
		{
			_file_path = file_path.str();
			return (SUCCESS);
		}
	}
	return (ERROR);
}

int	Client::checkExtension( std::string & root, std::string & url )
{
	int								pos = url.find(".");
	int								attachement = url.find("/download/");
	std::stringstream				file_path;

	if (pos > 0 && attachement < 0)
	{
		Value		location = _config[_hostname]["location"];
		std::string	extension = url.substr(pos);
		std::string	path = location._locations[extension];
		if (path.size())
			url = path + url;
	}
	file_path << root << url;
	if (openFile(file_path.str()) > 0)
	{
		_file_path = file_path.str();
		return (SUCCESS);
	}
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
	if (fd > 0)
	{
		close(fd);
		return (ERROR);
	}
	return SUCCESS;
}

int	Client::checkURI(std::string url)
{
	int					ret;
	std::string			root;

	if (url == "/")
		url = _config[_hostname]["index"]._value;
	root = _config[_hostname]["root"]._value;
	ret = checkCGI(url);
	if (checkPath(root, url) > 0)
		return (ret);
	if (checkExtension(root, url) > 0)
		return (ret);
	_status = NOT_FOUND;
	return (R_ERR);
}

int	Client::execution( Server const & serv, Port & port )
{
	int	res_type = ERROR;

	saveLogs();
	if (_status != OK)
		executeError();
	else
	{
		res_type = checkURI(_req.getHead()["url"]);
		std::cout << "PATH " << _file_path << std::endl;
		if (res_type == R_PHP || res_type == R_PY)
			executePhpPython(serv, port, res_type);
		else if (res_type == R_HTML)
			executeHtml();
		else if (res_type == R_ERR)
			executeError();
	}
	return SUCCESS;
}

int Client::executePhpPython( Server const & serv, Port & port, int extension_type)
{
	CGI cgi(*this, port, serv, extension_type);
	cgi.execute(*this);
	return SUCCESS;
}

int	Client::executeHtml( void )
{
	std::string	content;
	char		*line = NULL;
	size_t		n = 0;
	FILE *		file = fopen(_file_path.c_str(), "r");
	ssize_t		ret = 0;

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

int	Client::executeError( void )
{
	Value							error = _config[_hostname]["error_page"];
	std::string						error_file_path = error._errors[_status];
	std::string						body;
	std::string						root;


	root = _config[_hostname]["root"]._value;
	root += error_file_path;
	int ret = openFile(root);
	if (error_file_path.size() && ret > 0)
	{
		checkURI(error_file_path);
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
map_configs							Client::getConfig( void ) const { return _config; }
std::string							Client::getHostname( void ) const { return _hostname; }

}