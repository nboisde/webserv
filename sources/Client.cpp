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
Client::Client( void ) :
_fd(0),
_status(0),
_ip(""),
_port(""),
_req(),
_res(),
_file_path(""),
_config(),
_errors(),
_hostname(""),
_extension("")
{}

Client::Client( int fd, struct sockaddr_in *cli_addr, map_configs conf ) : 
_fd(0),
_status(0),
_ip(""),
_port(""),
_req(),
_res(),
_file_path(""),
_config(),
_errors(),
_hostname(""),
_extension("")
{
	_fd = fd;
	_status = OK;
	_ip = inet_ntoa(cli_addr->sin_addr);
	std::stringstream port;
	port << ntohs(cli_addr->sin_port);
	_port += port.str();
	_config = conf;
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
		this->_ip = rhs.getIp();
		this->_port = rhs.getPort();
		this->_req = rhs.getReq();
		this->_res = rhs.getRes();
		this->_file_path = rhs.getFilePath();
		this->_route = rhs._route;
		this->_config = rhs.getConfig();
		this->_errors = rhs._errors;
		this->_hostname = rhs._hostname;
		this->_extension = rhs._extension;
		this->_url = rhs._url;
		this->_root = rhs._root;

	}
	return *this;
}

#include <sys/types.h>
#include <sys/stat.h>

// SI FORMULAIRE GERE PAR CGI, EDITER _HEAD...
int Client::uploadFiles( std::string upload_path)
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
			int pos = _file_path.find(_route.route);
			std::string path = _file_path.substr(0, pos + _route.route.size()) + upload_path;
			std::cout << "UPLOAD PATH " << path << std::endl;
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
                f_name = path + "/" + f_name;
			
			std::cout << DEV << f_name << RESET << std::endl;
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

int	Client::setHostname( void )
{
	std::string tmp2 = _req.getHead()["host"];
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
	return (0);
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
		if (setHostname())
			return WRITING;
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

	for (; it != ite; it++)
	{
		int ret = 0;
		if ((ret = _file_path.find(it->first)) >= 0)
		{
			_route = Route(it->second);
			return;
		}
	}
	_route = Route();
}

int	Client::checkCGI( void )
{

	std::cout << YELLOW <<_file_path << RESET << std::endl; 

	size_t		pos = _file_path.find(".");

	std::cout << _file_path << std::endl;
	std::cout << static_cast<size_t>(-1);
	std::cout << "POS: " << pos << std::endl;
	if (pos == static_cast<size_t>(-1))
		return 0;
	size_t i = _file_path.size() - 1;
	for (i = _file_path.size() - 1; (_file_path[i] != '.'); i--);
	_extension = _file_path.substr(i);

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

int Client::checkRedirection( void )
{
	if (_route.route == "")
		return (0);
	std::string redirection = _route.redirection;
	if (redirection == "")
		return (0);
	int	pos = _file_path.find(_route.route);
	if (pos >= 0)
	{
		std::string new_url = redirection + _file_path.substr(pos + _route->route.size());
		_file_path = new_url;
		std::cout << BLUE << "NEW FILE " << _file_path << RESET << std::endl;
		return (R_REDIR);
	} 
	return 0;
}

int Client::checkMethod( void )
{
	std::string method = _req.getHead()["method"];

	if (_route.route != "")
	{
		std::vector<std::string>::iterator it = _route.methods.begin();
		std::vector<std::string>::iterator ite = _route.methods.end();
		for (; it != ite; it++)
			if (method == *it)
				return (SUCCESS);
	}
	std::vector<std::string>::iterator it = _config[_hostname]["method"]._methods.begin();
	std::vector<std::string>::iterator ite = _config[_hostname]["method"]._methods.end();
	for (; it != ite; it++)
		if (method == *it)
			return (SUCCESS);
	_status = NOT_ALLOWED;
	return (0);
}

int	Client::checkAutoindex( void )
{
	if (!isURLDirectory())
		return 0;
	else if (_route.route != "" && _route.index != "")
	{

		std::string index = _config[_hostname]["root"]._value + _route.index;
		_file_path = index;
		return 0;
	}
	else if (_route.route != "" && _route.autoindex == "on")
		return R_AUTO;
	_status = FORBIDDEN;
	return R_ERR;
}

int	Client::checkUpload( void )
{
	if (_req.getMultipart() == 1)
	{	
		if (_route.route != "" && _route.upload != "")
			uploadFiles(_route.upload);
		else if (_config[_hostname]["upload"]._value != "")
			uploadFiles(_config[_hostname]["upload"]._value);
		else
			return R_ERR;
	}
	return 0;
}

int	Client::checkPath( void )
{
	int fd = ::open(_file_path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		_status = NOT_FOUND;
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
	if (fd >= 0)
		close(fd);
    std::string local_host_name = inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
	return local_host_name;
}

int Client::isURLDirectory( void )
{
	int fd = ::open(_file_path.c_str(), O_DIRECTORY);
	if (fd > 0)
	{
		close(fd);
		return (1);
	}
	return (0);
}

int Client::executeAutoin( void )
{
	std::string loc = _file_path;

	listdir ld;

	if (_route.route != "")
	{
		_file_path = _config[_hostname]["root"]._value + _route.route;
		_res.setBody(ld.generateAutoindex(_file_path, _route.route));
		_res.setContentType(_file_path);
		_res.setContentDisposition(_file_path);
		_res.response(_status);
		return SUCCESS;
	}
	_status = NOT_FOUND;
	return 0;
}

void	Client::setPath( void )
{
	_url = _req.getHead()["url"];
	_root = _config[_hostname]["root"]._value;

	if (_url == "/")
		_url = _config[_hostname]["index"]._value;
	_file_path = _root + _url;
	int query = _file_path.find("?");
	if (query != -1)
		_file_path = _file_path.substr(0, query);
}

int Client::execution( Server const & serv, Port & port)
{
	_file_path = _config[_hostname]["root"]._value + _req.getHead()["url"];

	saveLogs();
	setPath();
	setRoute();

	std::cout << DEV << "FILE PATH = " << _file_path << std::endl;
	if (_route.route != "")
		std::cout << "ROUTE = " << _route.route << RESET << std::endl;
	else
		std::cout << "ROUTE = EMPTY" << RESET << std::endl;
	int ret = setExecution();
	if (ret == R_EXT)
		executeExtension(serv, port);
	else if (ret == R_AUTO)
		executeAutoin();
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
	if (error_file_path.size())
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