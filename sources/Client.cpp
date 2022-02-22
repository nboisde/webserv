#include "Client.hpp"
# include "Value.hpp"
namespace ws
{

	typedef	std::map<std::string, Value>		config_type;
	typedef std::map<int, std::string>			error_type;
	typedef std::map<std::string, config_type>	map_configs;

/*
** ------------------------------- CONSTRUCTOR / DESTRUCTOR--------------------
*/ 
Client::Client( void ) {}

Client::Client( int fd, struct sockaddr_in *cli_addr, map_configs conf ) : _fd(fd), _status(OK), _config(conf)
{
	_ip = inet_ntoa(cli_addr->sin_addr);
	std::stringstream port;
	port << ntohs(cli_addr->sin_port);
	_port += port.str();
	std::cout << "DEFAULT HOST " << _config.begin()->second["server_name"]._value << std::endl;
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

std::string Client::uploadPath( std::string url)
{
	std::string	upload_path;
	Value		location = _config[_hostname]["location"];
	Route		route = (location._locations)[url];
 	
	upload_path = route.upload; 
	return (upload_path);
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
			
			std::string path = uploadPath("route");
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

int Client::uploadAuthorized( void )
{
	for (std::vector<std::string>::iterator it = _config[_hostname]["method"]._methods.begin(); it != _config[_hostname]["method"]._methods.end(); it++)
	{
		if ((*it) == "POST")
			return 1;
	}
	return 0;
}

int Client::receive( void )
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
	if (req == -1 && _req.findContinue() == 0)
	{
		std::cout << RED << "400 bad request (Header reception 1)" << RESET << std::endl;
		_status = _req.getStatus();
		return WRITING;
	}
	if (req == SUCCESS)// && _req.getContinue() == 0)
	{
		int head_err = _req.fillHeaderAndBody();
		std::string tmp2 = _req.getHead()["host"];
		std::cout << tmp2 << std::endl;
		size_t pos = tmp2.find(":");
		if (pos >= static_cast<size_t>(0))
			_hostname = tmp2.substr(0, pos);
		else
			_hostname = tmp2;
		map_configs::iterator it = _config.end();
		if (_config.find(_hostname) == it && _hostname != LOCALHOST)
		{
			if (_hostname == "127.0.0.1" || _hostname == getLocalHostname())
				_hostname = LOCALHOST;
			else
			{
				_status = BAD_REQUEST;
				return WRITING;
			}
				it--;
				_hostname = it->second["server_name"]._value;
		}
		if (_hostname == LOCALHOST)
		{
			it--;
			_hostname = it->second["server_name"]._value;
		}
		_req.setUploadAuthorized(this->uploadAuthorized());
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

void Client::bridgeParsingRequest( void )
{
	int not_all = 1;

	for (std::vector<std::string>::iterator it = _config[_hostname]["method"]._methods.begin(); it != _config[_hostname]["method"]._methods.end(); it++)
	{
		std::cout << (*it) << std::endl;
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

int 	Client::checkLocation( std::string & url, std::string & route)
{
	Value									location = _config[_hostname]["location"];
	std::map<std::string, Route>::iterator	it = location._locations.begin();
	std::map<std::string, Route>::iterator	ite = location._locations.end();
	
	for (; it != ite; it++)
	{
		if (url.find(it->first) >= 0)
		{
			route = it->first;
			return (1);
		}
	}
	return (0);
}

int	Client::checkPath( std::string & root, std::string & url )
{
	std::stringstream				file_path;
	
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

std::string Client::getLocalHostname( void ) const
{
	int fd;
	struct ifreq ifr;
	
    // replace with your interface name
    // or ask user to input
    
	char iface[] = "enp3s0f0";
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	//Type of address to retrieve - IPv4 IP address
	ifr.ifr_addr.sa_family = AF_INET;
	//Copy the interface name in the ifreq structure
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
    std::string local_host_name = inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
	return local_host_name;
}

int	Client::checkURI( std::string url)
{
	int					ret;
	std::string			root;

	if (url == "/")
		url = _config[_hostname]["index"]._value;
	root = _config[_hostname]["root"]._value;
	ret = checkCGI(url);
	if (checkPath(root, url) > 0)
	{
		std::cout << RED << "RET " << ret << RESET << std::endl;
		return (ret);
	}
	_status = NOT_FOUND;
	std::cout << RED << "RET " << R_ERR << RESET << std::endl;
	return (R_ERR);
}

int	Client::execution( Server const & serv, Port & port )
{
	int	res_type = ERROR;

	saveLogs();
	if (_status != OK)
		executeError(_req.getHead()["url"]);
	else
	{
		res_type = checkURI(_req.getHead()["url"]);
		std::cout << RED << "URL " << _req.getHead()["url"] << RESET << std::endl;
		std::cout << RED << "RET " << res_type << RESET << std::endl;
		if (res_type == R_PHP || res_type == R_PY)
			executePhpPython(serv, port, res_type);
		else if (res_type == R_HTML)
			executeHtml();
		else if (res_type == R_ERR)
			executeError(_req.getHead()["url"]);
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

int	Client::executeRedir( std::string new_path)
{
	checkURI(new_path);
	_status = MOVED_PERMANETLY;
	_res.resetResponse();
	std::string loc("Location: ");
	loc += new_path;
	_res.setHeader(loc);
	_res.response(_status);
	return SUCCESS;
}

int	Client::executeError( std::string url )
{

	Value			location = _config[_hostname]["location"];

	std::string		route = "";

	if (checkLocation(url, route))
	{
		std::string redirection = location._locations[route].redirection;
		if (redirection != "")
		{
			int	pos = url.find(route);
			std::cout << "POS " << pos << std::endl;
			if (pos >= 0)
			{
				std::string new_url = url.substr(0, pos);
				new_url += redirection;
				new_url += url.substr(pos + route.size());
				executeRedir(new_url);
				return (SUCCESS);
			}
		} 
	}
	Value				error = _config[_hostname]["error_page"];
	std::string			error_file_path = error._errors[_status];
	std::string			body;
	std::string			root;

	root = _config[_hostname]["root"]._value;
	root += error_file_path;
	int ret = openFile(root);
	if (error_file_path.size() && ret > 0)
		executeRedir(error_file_path);
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