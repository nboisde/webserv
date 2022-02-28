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
_extension(""),
_file_complete(true),
_tmp_file(NULL),
_upload_fd(-1)
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
_extension(""),
_file_complete(true),
_tmp_file(NULL),
_upload_fd(-1)
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

void	Client::resetValues( void ){
_status = 0;
_ip.clear();
_ip = "";
_port.clear();
_port = "";
_file_path.clear();
_file_path = "";
_hostname.clear();
_hostname = "";
_extension.clear();
_extension = "";
_file_complete = true;
_tmp_file = NULL;
_upload_fd = -1;
}

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

		//GUIGS POLLPATCH
		this->_file_complete = rhs.getFileFlag();
		this->_tmp_file = rhs.getTmpFile();
		this->_upload_fd = rhs.getUploadFd();

	}
	return *this;
}

#include <sys/types.h>
#include <sys/stat.h>

// SI FORMULAIRE GERE PAR CGI, EDITER _HEAD...
int Client::uploadFiles( void )
{
	//SET UP DE LA ROUTE DU FICHIER
	std::string upload_path = "";
	if (_route.route != "" && _route.upload != "")
		upload_path = _route.upload;
	else if (_config[_hostname]["upload"]._value != "")
		upload_path = _config[_hostname]["upload"]._value;

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

int	Client::setHostname( void )
{
	std::string tmp2 = _req.getHead()["host"];
	size_t pos = tmp2.find(":");
	if (pos >= static_cast<size_t>(0))
		_hostname = tmp2.substr(0, pos);
	else
		_hostname = tmp2;
	map_configs::iterator it = _config.find(_hostname);
	map_configs::iterator ite = _config.end();
	if (it == ite && _hostname != LOCALHOST)
	{
		if (_hostname == "127.0.0.1" || _hostname == getLocalHostname())
			_hostname = LOCALHOST;
		else
		{
			_status = BAD_REQUEST;
			return WRITING;
		}
	}
	else if (it != ite)
		_hostname = it->second["server_name"]._value;
	if (_hostname == LOCALHOST)
		_hostname = _config.begin()->second["server_name"]._value;
	return (0);
}

int Client::receive( void )
{
	char	buffer[BUFFER_SIZE];

	memset(buffer, 0, BUFFER_SIZE);
	if (!_file_complete)
		return WRITING;
	int ret = recv(_fd, buffer, BUFFER_SIZE - 1, 0);
	if ( ret < 0)
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
	if (req == SUCCESS)
	{
		int head_err = _req.fillHeaderAndBody();
		if (setHostname())
			return WRITING;
		_req.setContinue(0);
		if (head_err == ERROR)
		{
			std::cout << RED << "400 bad request (Header reception 2)" << RESET << std::endl;
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
		return CLOSING;
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
	size_t		pos = _file_path.find(".");

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
		std::string new_url = redirection + _file_path.substr(pos + _route.route.size());
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
		{
			if (method == *it)
			{
				if (method == "DELETE")
					return R_DELETE;
				else
					return (0);
			}
		}
	}
	std::vector<std::string>::iterator it = _config[_hostname]["method"]._methods.begin();
	std::vector<std::string>::iterator ite = _config[_hostname]["method"]._methods.end();
	for (; it != ite; it++)
	{
		if (method == *it)
		{
			if (method == "DELETE")
				return R_DELETE;
			else
				return (0);
		}
	}
	_status = NOT_ALLOWED;
	return (R_ERR);
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
	if (_req.getMultipart() == 1) // ADD METHOD = POST
	{
		// if (_route.route != "" && _route.upload != "")
		// 	uploadFiles(_route.upload);
		// else if (_config[_hostname]["upload"]._value != "")
		// 	uploadFiles(_config[_hostname]["upload"]._value);
		// else
		if (_route.route != "" && _route.upload != "")
			return R_UPLOAD;
		else if (_config[_hostname]["upload"]._value != "")
			return R_UPLOAD;
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
	std::string	loc = _file_path;
	listdir 	ld;

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

bool Client::TmpFileCompletion(Server & serv)
{
	std::map<std::string, std::string> header = _req.getHead();
	if (header["method"] == "POST")
	{
		//NEXT CHECK IF FD ALREADY EXIT, IF NOT, CREATE AND ADD TO POLL WITH POLLOUT FLAG
		if (_tmp_file == NULL)
		{
			_tmp_file = tmpfile();
			serv.addToPolling(fileno(_tmp_file));
			serv.findFds(fileno(_tmp_file)).events = POLLOUT;
			return false;
		}
		else if (serv.findFds(fileno(_tmp_file)).revents & POLLOUT)
		{
			std::string body = _req.getBody();
			if (!body.empty())
			{
				char buf[BUFFER_SIZE];
				memset(&buf, 0, BUFFER_SIZE);
				size_t added = body.copy(buf, BUFFER_SIZE);
				size_t ret = fwrite(buf, sizeof(char), added, _tmp_file);
				_req.setBody(body.substr(ret));
				return false;
			}
			else
			{
				rewind(_tmp_file);
				serv.setCleanFds(true);
				serv.findFds(fileno(_tmp_file)).fd = -1;
				return true;
			}
		}
		else
			return false;
	}
	else
		return true;
}

int	Client::delete_ressource( void )
{
	std::cout << "FILE_PATH IM GOING TO DELETE = " << _file_path << std::endl;
	
	//CHECKING IF IS FILE: IF NOT, OUT
	if (!isfile(_file_path))	
		return ERROR;

	//RESOLVED_PATH (no symlynk or ./ or ../)
	char * tmp = NULL;
	tmp = realpath(_file_path.c_str(), NULL);	
	std::string real_path(tmp);
	free(tmp);
	real_path = real_path.substr(0, real_path.find_last_of("/"));
	std::cout << "PATH_CHECK =" << real_path << std::endl;
	
	std::map<std::string, Route> ml = _config[_hostname]["location"]._locations;
	tmp = realpath(_config[_hostname]["root"]._value.c_str(), NULL);
	std::string authorized_path(tmp);
	free(tmp);
	authorized_path += "/deletable";
	//authorized_path += ml.find("upload")->second;
	std::cout << "AUTHORIZED_PATH =" << authorized_path << std::endl;
	if (real_path != authorized_path)
		return ERROR;
	if (!remove(_file_path.c_str()))
	{
		_res.response(OK);
		return SUCCESS;
	}
	else
		return (executeError());
}

int Client::execution( Server & serv, Port & port)
{
	_file_path = _config[_hostname]["root"]._value + _req.getHead()["url"];

	saveLogs();
	setPath();
	setRoute();

	std::cout << _req.getHeader() << std::endl;
	
	int exec_type = setExecution();
	if (exec_type == R_EXT)
	{
		//CHECK IF TMP_FILE IS NEEDED, AND MONITOR IT WITH POLL IF SO
		if (!TmpFileCompletion(serv))
		{
			_file_complete = false;
			return SUCCESS;
		}
		executeExtension(serv, port);
	}
	// else if (exec_type == R_UPLOAD)
	// {
	// 	if (!uploadFiles())
	// 	{
	// 		_file_complete = false;
	// 		return SUCCESS;
	// 	}
	// 	_file_complete = true;
	// }
	else if (exec_type == R_AUTO)
		executeAutoin();
	else if (exec_type == R_HTML)
		executeHtml();
	else if (exec_type == R_REDIR)
		executeRedir();
	else if (exec_type == R_DELETE)
		delete_ressource();
	else
		executeError();
	_file_complete = true;
	return SUCCESS;
}

int	Client::setExecution( void )
{
	int ret;

	if (_status != OK)
		return R_ERR;
	if ((ret = checkRedirection()))
		return ret;
	if ((ret = checkMethod()))
		return ret;
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


int Client::executeExtension( Server & serv, Port & port)
{
	CGI cgi(*this, port, serv);
	cgi.execute(*this);
	if (_tmp_file)
		fclose(_tmp_file);
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

//GUIGS PATCH
bool								Client::getFileFlag(void) const {return _file_complete;}
void								Client::setFileFlag(bool new_bool) {_file_complete = new_bool;}
FILE*								Client::getTmpFile( void) const {return _tmp_file;}
int									Client::getUploadFd( void ) const { return _upload_fd;}
}