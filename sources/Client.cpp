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
	if (static_cast<size_t>(_req.getBody().length()) > _config[_hostname]["client_max_body_size"]._max_body_size
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

void 	Client::setRoute( void )
{
	Value									location = _config[_hostname]["location"];
	std::map<std::string, Route>::iterator	it = location._locations.begin();
	std::map<std::string, Route>::iterator	ite = location._locations.end();
	_route = NULL;

	for (; it != ite; it++)
	{
		if (_file_path.find(it->first) >= 0)
			_route = &(it->second);
	}
}

int	Client::checkCGI( std::string & url )
{
	int		query_pos = url.find("?");
	int		php_pos = _file_path.find(".php");
	int		py_pos = _file_path.find(".py");
	int		size = _file_path.size();
	int		query_pos = _file_path.find("?");

	if ( query_pos >= 0 )
			url = url.substr(0, query_pos);
	size_t i = url.size() - 1;
	for (i = url.size() - 1; url[i] != '.'; i--);
	_extension = url.substr(i);

	//Cette partie du code parcourt les extensions que possede notre fichier de conf
	Value const & cgi = _config[_hostname]["cgi"];
	std::map<std::string, std::string> const & extensions = cgi._list;
	std::map<std::string, std::string>::const_iterator map_it = extensions.begin();
	std::map<std::string, std::string>::const_iterator map_end = extensions.end();
	for (;map_it != map_end; map_it++)
	{
		if (_extension == map_it->first)
			return R_EXT;
	}
	return (R_HTML);
}

int	Client::checkPath( void )
{
	int fd = ::open(_file_path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		_status = BAD_REQUEST;
		return 0;
	}
	close(fd);
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

int Client::isURLDirectory( std::string url )
{
	std::string path = _config[_hostname]["root"]._value + url;
	//std::cout << DEV << path << std::endl;
	int fd = ::open(path.c_str(), O_DIRECTORY);
	std::cout << YELLOW << "Directory " << fd << RESET << std::endl;
	if (fd > 0)
	{
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

int Client::executeAutoin( std::string url, Server const & serv, Port & port )
{
	int	res_type = ERROR;
	std::string loc = url;
	std::string route = "";
	if (!checkLocation(loc, route))
	{
		_status = FORBIDDEN;
		executeError(_req.getHead()["url"]);
		return SUCCESS;
	}
	listdir ld;
	if (route != "" && _config[_hostname]["location"]._locations[route].index != "")
	{
		std::string index = _config[_hostname]["root"]._value + url + "/" + _config[_hostname]["location"]._locations[route].index;
		_file_path = index;
		_req.setHeadKey("url", url + "/" + _config[_hostname]["location"]._locations[route].index);
		int check_existance  = ::open(index.c_str(), O_RDONLY);
		if (check_existance < 0)
		{
			_status = NOT_FOUND;
			res_type = R_ERR;
			executeError(index);
			return SUCCESS;
		}
		res_type = checkCGI(index);
		if (res_type == R_EXT)
			executeExtension(serv, port);
		else if (res_type == R_HTML)
			executeHtml();
		else if (res_type == R_ERR)
		{
			executeError(_req.getHead()["url"]);
		}
		return SUCCESS;
	}
	else if (route != "" && _config[_hostname]["location"]._locations[route].autoindex == "on")
	{
		_res.setBody(ld.generateAutoindex(_config[_hostname]["root"]._value + route, route));
		_file_path = _config[_hostname]["root"]._value + route;
		_res.setContentType(_file_path);
		_res.setContentDisposition(_file_path);
		_res.response(_status);
		return SUCCESS;
	}
	else
	{
		_status = FORBIDDEN;
		executeError(_req.getHead()["url"]);
	}
	return SUCCESS;
}

int	Client::checkURI( std::string url)
{
	std::string method = _req.getHead()["method"];

	if (url == "/")
		url = _config[_hostname]["index"]._value;
	if (isURLDirectory(url))
	{
		_status = OK;
		return R_AUTO;
	}
	root = _config[_hostname]["root"]._value;
	ret = checkCGI(url);
	if (checkPath(root, url) > 0)
		return (ret);
	_status = NOT_FOUND;
	return (R_ERR);
	std::vector<std::string>::iterator it = _config[_hostname]["method"]._methods.begin();
	std::vector<std::string>::iterator ite = _config[_hostname]["method"]._methods.end();
	for (; it != ite; it++)
		if (method == *it)
			return (SUCCESS);
	_status = BAD_REQUEST;
	return (0);
}

void	Client::setPath( void )
{
	std::string url = _req.getHead()["url"];
	std::string	root = _config[_hostname]["root"]._value;

	if (url == "/")
		url = _config[_hostname]["index"]._value;
	_file_path = root + url;
	int query = _file_path.find("?");
	_file_path = _file_path.substr(0, query);
}

int Client::execution( Server const & serv, Port & port)
{
	int	res_type = ERROR;
	_file_path = _config[_hostname]["root"]._value + _req.getHead()["url"];

	saveLogs();
	setPath();
	setRoute();
	
	int ret = setExecution();
	if (ret == R_PHP || ret == R_PY)
		executePhpPython(serv, port, ret);
	else if (ret == R_AUTO)
		executeAuto();
	else if (ret == R_HTML)
		executeHtml();
	else if (ret == R_REDIR)
		executeRedir();
	else
		executeError();
	return SUCCESS;
}

int	Client::setExecution( void )
{
	int ret;

	if (_status != OK)
		return R_ERR;
	if ((ret = checkRedirection()))
		return ret;
	if (!checkMethod())
		return R_ERR;
	 if ((ret = checkAutoindex()))
	 	return ret;
	if (!checkPath())
		return R_ERR;
	 if ((ret = checkUpload()))
	 	return ret;
	if ((ret = checkCGI()))
		return ret;
	return R_HTML;

}

int Client::executeExtension( Server const & serv, Port & port)
{
	CGI cgi(*this, port, serv);
	cgi.execute(*this);
	return SUCCESS;
}

void	Client::executeAuto( void )
{

}

void	Client::executeRedir( void )
{
	_status = MOVED_PERMANETLY;
	_res.resetResponse();
	std::string loc("Location: ");
	loc += _file_path;
	_res.setHeader(loc);
	_res.response(_status);
}

void	Client::executeHtml( void )
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
}

int	Client::executeError( void )
{

	Value			location = _config[_hostname]["location"];
	Value			error = _config[_hostname]["error_page"];
	std::string		error_file_path = error._errors[_status];
	std::string		body;
	std::string		root;

	root = _config[_hostname]["root"]._value;
	root += error_file_path;
	int ret = openFile(root);
	if (error_file_path.size() && ret > 0)
	{
		_file_path = error_file_path;
		executeRedir();
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

void Client::closeConnection(){
	_status = OK;
}

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
map_configs const & 				Client::getConfig( void ) { return _config; }
std::string							Client::getHostname( void ) const { return _hostname; }
std::string const & 				Client::getExtension( void ) const {return _extension;}
}