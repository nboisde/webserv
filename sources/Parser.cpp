#include "Parser.hpp"

namespace ws
{

Parser::Parser(void) : _pos(0), _server("127.0.0.1") 
{
	defaultConfiguration(); 
}
Parser::~Parser(void) {}

int	Parser::launch(std::string file)
{
	_config_file = file;
	if (!checkFileName())
		return (0);
	if (!readFile())
		return (0);
	if (!initWebServer())
		return (0);
	return (1);
}

int	Parser::checkFileName(void)
{
	int size = _config_file.size();
	int pos = _config_file.rfind(".conf");
	if (pos == -1)
		return (0);
	else if ((size - pos) != 5)
		return (0);
	return (1);
}

int	Parser::readFile(void)
{
	std::ifstream	ifs(_config_file.c_str());
	std::string		buffer;

	while (getline(ifs, buffer))
		_content += buffer;
	_size = _content.size();
	ifs.close();
	return (1);
}

int	Parser::initWebServer(void)
{
	if (!checkHttp())
		return (0);
	while (_pos < _size)
	{
		if (!checkServer())
			return (0);
		while (_pos < _size && isspace(_content[_pos]))
			_pos++;
		if (_content[_pos] == '}')
		{
			_pos++;
			break;
		}
	}
	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	if (_pos != _size)
		return (0);
	return (1);
}

int	Parser::checkHttp(void)
{
	int	i;

	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	i = _content.find("http");
	if (_pos != i)
		return (0);
	_pos += 4;
	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	if (_content[_pos] != '{')
		return (0);
	_pos++;
	return (1);
}

int	Parser::checkServer(void)
{
	int i;

	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	i = _content.find("server", _pos);
	if (i != _pos)
		return (0);
	_pos += 6;
	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	if (_content[_pos] != '{')
		return (0);
	_pos++;
	_server.addPort(Port(_dict));
	while (_pos < _size)
	{
		if (!checkKeys())
			return (0);
		while (_pos < _size && isspace(_content[_pos]))
			_pos++;
		if (_content[_pos] == '}')
		{
			_pos++;
			break;
		}
	}
	if (_pos == _size)
		return (0);
	return (1);
}


int	Parser::checkKeys(void)
{
	std::map<std::string, std::string>::iterator it = _dict.begin();
	std::map<std::string, std::string>::iterator ite = _dict.end();
	int	found = 0;

	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	for (; it != ite; it++)
	{
		int i = _content.find((*it).first, _pos);
		if (i == _pos)
		{
			found = 1;
			_pos += (*it).first.size();
			break;
		}
	}
	if (!found)
		return (0);
	if (!setValues((*it).first))
		return (0);
	return (1);
}

int	Parser::setValues(std::string key)
{
	int	isspaceNb = 0;

	while (_pos < _size && isspace(_content[_pos]))
	{
		_pos++;
		isspaceNb++;
	}
	if (!isspaceNb)
		return (0);
	while (_pos < _size && _content[_pos] != ';')
	{
		int	dot = _content.find_first_of(";", _pos);
		std::string value = _content.substr(_pos, dot - _pos);
		if (!(checkValue(key, value, _server.getRefPorts().back())))
			return (0);
		_pos = dot;
	}
	if (_pos == _size)
		return (0);
	_pos++;
	return (1);
}

int	Parser::checkPort(std::string value)
{
	int port = atoi(value.c_str());
	std::cout << "Port " << port << std::endl;
	if (port < 0 || port > 65535)
		return (0);
	else
		return (1);
}

int	Parser::checkMethod(std::string value)
{
	std::string	method;
	int			position;

	while (value.size())
	{
		position = value.find("|");
		std::cout << "position " << position << std::endl;
		std::cout << "method " << method << std::endl;
		std::cout << "value " << value << std::endl;
		value = value.substr(position);
		if (method != "GET" || method != "POST" || method != "DELETE")
			return (0);
		value.substr(method.size());
	}
	return (1);
}

int	Parser::checkAutoindex(std::string value)
{
	if (value == "on")
		return (1);
	else if (value == "off")
		return (1);
	else
		return (0);
}

int	Parser::checkClientMaxSize(std::string value) { (void)value; return (1); }
int	Parser::checkHost(std::string value) { (void)value; return (1); }
int	Parser::checkServerName(std::string value) { (void)value; return (1); }
int	Parser::checkErrorPage(std::string value) { (void)value; return (1); }
int	Parser::checkRoot(std::string value) { (void)value; return (1); }
int	Parser::checkIndex(std::string value) { (void)value; return (1); }

int	Parser::checkValue(std::string key, std::string value, Port & port)
{
	std::map<std::string, std::string>::iterator	ite = port.getConfig().end();
	std::map<std::string, std::string>::iterator	it = port.getConfig().find(key);
	std::map<std::string, validity_fct>::iterator	cite = _validity_check.end();
	std::map<std::string, validity_fct>::iterator	cit = _validity_check.find(key);

	if (it == ite || cite == cit)
		return (0);
	if (!((this->*_validity_check[key])(value)))
	{
		std::cout << key << " : " << value << " is not valid" << std::endl;
		return (0);
	}
	_server.getRefPorts().back().getConfig()[key] = value;
	return (1);
}

int Parser::defaultConfiguration(void)
{
	std::ifstream file("./conf/dictConf.default");
	std::string line;
	while (std::getline(file, line))
	{
		int i = 0;
		while (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\v')
			i++;
		if (line[i] == '#')
			continue ;
		else if (!line[i] || line[i] == '\n')
			continue ;
		else
		{
			int dbl = line.find(":");
			if (static_cast<int>(dbl) == -1)
				return (1);
			std::string key = line.substr(i, dbl - i);
			std::string value = line.substr(dbl + 1, line.length());
			while (static_cast<int>(key.find(" ")) != -1 || static_cast<int>(key.find("\t")) != -1)
				key = key.substr(0, key.length() - 1);
			int k = 0;
			while (value[k] == ' ' || value[k] == '\t' || value[k] == '\r' || value[k] == '\v')
				k++;
			value = value.substr(k, value.length() - k);
			while (static_cast<int>(value.find(" ")) != -1 || static_cast<int>(value.find("\t")) != -1)
				value = value.substr(0, value.length() - 1);
			_dict[key] = value;
		}
	}
	_validity_check["listen"] = &Parser::checkPort;
	_validity_check["host"] = &Parser::checkHost;
	_validity_check["server_name"] = &Parser::checkServerName;
	_validity_check["client_max_body_size"] = &Parser::checkClientMaxSize;
	_validity_check["error_page"] = &Parser::checkErrorPage;
	_validity_check["autoindex"] = &Parser::checkAutoindex;
	_validity_check["method"] = &Parser::checkMethod;
	_validity_check["root"] = &Parser::checkRoot;
	_validity_check["index"] = &Parser::checkIndex;
	return SUCCESS;
}

Server	Parser::getServer( void ) { return _server; }

}