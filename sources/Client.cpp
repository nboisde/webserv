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
_cgi_complete(true),
_cgi_fd(-1),
_cgi_response(""),
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
_cgi_complete(true),
_cgi_fd(-1),
_cgi_response(""),
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
_cgi_complete = true;
_cgi_fd = -1;
_cgi_response.clear();
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
		this->_cgi_complete = rhs.getCGIFlag();
		this->_cgi_fd = rhs.getCGIFlag();
		this->_cgi_response = rhs.getCGIResponse();

	}
	return *this;
}

// I'LL NEED THAT SHIT TOMORROW TO DEBUG A CASE !!!! if upload path don't exist webserv hang forever...

// std::string Client::uploadPath( void )
// {
// 	std::map<std::string, std::string> ml = _config["location"]._locations;
// 	std::string s = "";
// 	if (ml.find("upload") == ml.end())
// 		return s;
// 	struct stat info;
// 	if (stat( ml["upload"].c_str(), &info) != 0)
// 	{
// 		std::cout << RED << "upload directory dosn't exists" << RESET << std::endl;
// 		std::cout << GREEN << "File will be registered by default at the root of the server." << RESET << std::endl;
// 	}
// 	else if (info.st_mode & S_IFDIR)
// 	{
// 		s += ml["upload"];
// 		s += '/';
// 	}
// 	else
// 	{
// 		std::cout << RED << "upload location in configuration is not a directory" << RESET << std::endl;
// 		std::cout << GREEN << "File will be registered by default at the root of the server." << RESET << std::endl;
// 	}
// 	std::cout << s << std::endl;
// 	return s;
// }

// CHECK SI LE FILE N'EXISTE PAS ?????
bool Client::uploadFiles(Server & serv)
{
	//SET UP DE LA ROUTE DU FICHIER
	std::string upload_path = "";
	if (_route.route != "" && _route.upload != "")
		upload_path =  _config[_hostname]["root"]._value + _route.route + _route.upload;
	else if (_config[_hostname]["upload"]._value != "")
		upload_path =  _config[_hostname]["root"]._value + "/" + _config[_hostname]["upload"]._value;

	std::string body = _req.getBody();
	next:
	if (!body.empty())
	{
		if (_upload_fd == -1)
		{
			std::string cls = "--" + _req.getBoundary() + "--";
			size_t end = body.find(cls);
			if (end == 0)// || lilend == 0)
				return true;
			size_t i_content = body.find("\r\n\r\n");
			std::string header;
			if (i_content == static_cast<size_t>(-1))
				header = body;
			else
				header = body.substr(body.find(_req.getBoundary()) + _req.getBoundary().length(), i_content - _req.getBoundary().length());
			if (header == "--")
				return true;
			size_t i_filename = header.find("filename") + 10;
			std::string tmp_filename = header.substr(i_filename);
			std::string filename = tmp_filename.substr(0, tmp_filename.find("\""));
			_req.setBody(body.substr(body.find("\r\n\r\n") + 4));
			body.clear();
			body = _req.getBody();
			size_t is_end = body.find(cls);
			if (filename.length() == 0 && is_end != 0)
			{
				std::string bnd_dash = "--" + _req.getBoundary();
				_req.setBody(body.substr(body.find(bnd_dash)));
				body.clear();
				body = _req.getBody();
				size_t end2 = body.find(cls);
				if (end2 == 0)
				{
					return true;
				}
				else
				{
					goto next;
					return false;
				}
			}
			if (filename.length() == 0 && is_end == 0)
				return true;
			std::string path = upload_path + "/" + filename;
			_upload_fd = ::open(path.c_str(), O_RDWR | O_CREAT);
			chmod(path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			serv.addToPolling(_upload_fd);
			serv.findFds(_upload_fd).events = POLLOUT;
			return false;
		}
		else if (serv.findFds(_upload_fd).revents & POLLOUT) // && fichier pas a la fin..)
		{
			std::string delim = "--" + _req.getBoundary();
			size_t end_file = body.find(delim);
			if (end_file > 0)
			{
				if (end_file > BUFFER_SIZE)
				{
					write(_upload_fd, body.c_str(), BUFFER_SIZE);
					_req.setBody(body.substr(BUFFER_SIZE));
				}
				else
				{
					write(_upload_fd, body.c_str(), end_file);
					_req.setBody(body.substr(end_file));
				}
				return false;
			}
			else
			{
				//std::string tmp = _req.getBody().substr(end_file, std::string::npos);
				//close(_upload_fd);
				if(_upload_fd != -1)
				{
					serv.setCleanFds(true);
					serv.findFds(_upload_fd).fd = -1;
					close(_upload_fd);
					_upload_fd = -1;
					goto next;
				}
				std::string bdy_end = "--" + _req.getBoundary() + "--";
				std::string tmp = _req.getBody();
				size_t upld_end = tmp.find(bdy_end);
				if (upld_end == 0)
				{
					tmp.clear();
					tmp = "";
					return true;
				}
				else
					return false;
			}
			//else
			//return true;
		}
		else
			return true;
	}
	return true;
}

//https://stackoverflow.com/questions/31783947/what-http-status-code-should-be-return-when-we-get-error-while-uploading-file
int Client::uploadAuthorized( void )
{
	std::string upload_path = "";
	if (_route.route != "" && _route.upload != "")
		upload_path =  _config[_hostname]["root"]._value + _route.route + _route.upload;
	else if (_config[_hostname]["upload"]._value != "")
		upload_path =  _config[_hostname]["root"]._value + "/" + _config[_hostname]["upload"]._value;

	struct stat info;
	if (stat(upload_path.c_str(), &info) != 0)
	{
		_status = INTERNAL_SERVER_ERROR;
		return 0;
	}
	else if (info.st_mode & S_IFDIR)
		return 1;
	else
	{
		_status = INTERNAL_SERVER_ERROR;
		return 0;
	}
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
	{
		map_configs::iterator it = _config.begin();
		map_configs::iterator ite = _config.end();
		_hostname = it->second["server_name"]._value;
		for (; it != ite; it++)
		{
			if (it->second["listen"]._default == true)
				_hostname = it->second["server_name"]._value;
		}
	}
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
		_req.cleanRawContent();
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
	std::string rewrite = _route.rewrite;
	if (rewrite == "")
		return (0);
	int	pos = _file_path.find(_route.route);
	if (pos >= 0)
	{
		std::string new_url = rewrite + _file_path.substr(pos + _route.route.size());
		_file_path = new_url;
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
	std::cout << _file_path << std::endl;
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
	_status = 404;
	return R_ERR;
}

int	Client::checkUpload( void )
{
	if (_req.getMultipart() == 1 && uploadAuthorized() == 1) // ADD METHOD = POST
	{
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
	{
		_status = NOT_ALLOWED;
		return (executeError());
	}

	if (!remove(_file_path.c_str()))
	{
		_res.response(NO_CONTENT);
		return SUCCESS;
	}
	else
	{
		_status = INTERNAL_SERVER_ERROR;
		return (executeError());
	}
}

void	Client::read_fd_out( Server & serv )
{
	if (_cgi_fd == -1)
		return;
	if (serv.findFds(_cgi_fd).revents & POLLIN)
	{
		char buf[BUFFER_SIZE];
		memset(&buf, 0, BUFFER_SIZE);
		read(_cgi_fd, &buf, BUFFER_SIZE);
		_cgi_response += buf;
		_cgi_complete = false;
	}
	else
	{
		close(_cgi_fd);
		serv.setCleanFds(true);
		serv.findFds(_cgi_fd).fd = -1;
		_cgi_complete = true;
	}
}

int Client::execution( Server & serv, Port & port)
{
	_file_path = _config[_hostname]["root"]._value + _req.getHead()["url"];

	saveLogs();
	setPath();
	setRoute();
	
	int exec_type = setExecution();
	if (exec_type == R_EXT)
	{
		std::cout << "ici";
		//CHECK IF TMP_FILE IS NEEDED, AND MONITOR IT WITH POLL IF SO
		if (!TmpFileCompletion(serv))
		{
			_file_complete = false;
			return SUCCESS;
		}
		executeExtension(serv, port);
		if (!_cgi_complete)
			read_fd_out(serv);
		else
		{
			_res.treatCGI(_cgi_response);
			_res.response(CGI_FLAG);
		}
	}
	else if (exec_type == R_UPLOAD)
	{
		if (!uploadFiles(serv))
		{
			_file_complete = false;
			return SUCCESS;
		}
		_file_path = _req.getHead()["url"];
		executeRedir();
		_file_complete = true;
	}
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
	if (!_cgi_complete)
		return SUCCESS;
	CGI cgi(*this, port, serv);
	cgi.execute(*this);

	//SETTING CGI COMPLETE TO FALSE, SINCE WE PASS HERE ONLY ONE TIME, OUT OF CGIs
	_cgi_complete = false;
	serv.addToPolling(_cgi_fd);
	serv.findFds(_cgi_fd).events = POLLIN;

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
std::string							Client::getRoot( void ) const { return _root;}
std::string							Client::getUrl( void ) const { return _url;}


//GUIGS PATCH
bool								Client::getFileFlag(void) const {return _file_complete;}
void								Client::setFileFlag(bool new_bool) {_file_complete = new_bool;}
FILE*								Client::getTmpFile( void) const {return _tmp_file;}
int									Client::getUploadFd( void ) const { return _upload_fd; }
bool								Client::getCGIFlag(void) const { return _cgi_complete; }
int									Client::getCGIFd( void ) const { return _cgi_fd; }
void								Client::setCGIFd( int newfd ) { _cgi_fd = newfd; }
std::string							Client::getCGIResponse( void ) const { return _cgi_response;}
}