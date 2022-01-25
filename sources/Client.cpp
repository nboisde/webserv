#include "Client.hpp"

namespace ws
{

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/ 
Client::Client( void ) {}
Client::Client( int fd ) : _fd(fd), _status(WRITING) {}
Client::Client( Client const & src ) { *this = src; }
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
	//std::cout << "RECEIVED CONTENT = [" << buffer << "]" << std::endl;
	//std::cout << buffer;
	if ( ret < 0 )
	{
		perror("\nIn recv");
		return WRITING;
	}
	std::string tmp(buffer, ret);
	int req = _req.concatenateRequest(tmp);
	if (req == -1)
	{
		std::cout << "Gerer ici une reponse d'erreur (400 -> Error Header.)" << std::endl;
		return WRITING;
		//exit(EXIT_FAILURE);
	}
	if (/* ret < BUFFER_SIZE - 1 ||  */req == 1)
	{
		//std::cout << "ret :" << ret << std::endl;
		int head_err = _req.fillHeaderAndBody();
		if (head_err == ERROR)
		{
			std::cout << "Gerer ici une reponse d'erreur (parsing du header problematique !) -> Retour d'erreur 400 -> bad request." << std::endl;
			return WRITING;
		}
 		//std::cout << _req.getRawContent() << std::endl;
		//std::cout << "--------------------------------" << std::endl << "Header:" << std::endl;
		std::cout << _req.getHeader() << std::endl;
		//std::cout << "Body : " << std::endl << _req.getBody() << std::endl;
		//std::cout << _req.getBody();// << std::endl;
		//std::cout << _req.getBody().length() << std::endl;
		write(1, _req.getBody().c_str(), _req.getBody().length());
		//std::cout << std::endl;
		return WRITING;
	}
	return READING;
}


int Client::send( void )
{
	int			ret;
	//int			len;
	const char* prov;
	std::string str = _res.response();

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
	std::map<std::string, std::string>::iterator	it = port.getConfig()["location"]._locations.find(url);
	std::map<std::string, std::string>::iterator	ite = port.getConfig()["location"]._locations.end();

	if (it != ite)
		url = (*it).second + (*it).first;
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
	std::cout << "URL " << url << std::endl;
	checkExtension(url, port);
	std::cout << "URL " << url << std::endl;
	root = port.getConfig()["root"]._value;
	buf = getcwd(buf, 0);
	file_path << buf << root << url;
	_file_path = file_path.str();
	std::cout << "PATH " << _file_path << std::endl;
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
	_res.response();
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
