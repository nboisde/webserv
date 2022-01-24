#include "Parser.hpp"

namespace ws
{

Parser::Parser(void) : _pos(0), _server("127.0.0.1") 
{ 
	_key_checker["listen"] = &Parser::checkPort;
	_key_checker["host"] = &Parser::checkHost;
	_key_checker["server_name"] = &Parser::checkServerName;
	_key_checker["client_max_body_size"] = &Parser::checkClientMaxSize;
	_key_checker["error_page"] = &Parser::checkErrorPage;
	_key_checker["autoindex"] = &Parser::checkAutoindex;
	_key_checker["method"] = &Parser::checkMethod;
	_key_checker["root"] = &Parser::checkRoot;
	_key_checker["index"] = &Parser::checkIndex;
	_key_checker["location"] = &Parser::checkLocation;
	_default_keys["listen"] = Value("8080");
	_default_keys["host"] = Value("Webserv.com");
	_default_keys["server_name"] = Value("webserv");
	_default_keys["client_max_body_size"] = Value("100M");
	_default_keys["error_page"] = Value("");
	_default_keys["autoindex"] = Value("off");
	_default_keys["method"] = Value("GET|POST|DELETE");
	_default_keys["root"] = Value("/www");
	_default_keys["index"] = Value("index.html");
	_default_keys["location"] = Value(""); 
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
	return (SUCCESS);
}

int	Parser::checkFileName(void)
{
	int size = _config_file.size();
	int pos = _config_file.rfind(".conf");
	if (pos == -1)
		return (0);
	else if ((size - pos) != 5)
		return (0);
	return (SUCCESS);
}

int	Parser::readFile(void)
{
	std::ifstream	ifs(_config_file.c_str());
	std::string		buffer;

	while (getline(ifs, buffer))
		_content += buffer;
	_size = _content.size();
	ifs.close();
	return (SUCCESS);
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
	return (SUCCESS);
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
	return (SUCCESS);
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
	_server.addPort(Port(_default_keys));
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
	return (SUCCESS);
}


int	Parser::checkKeys(void)
{
	std::map<std::string, Value>::iterator it = _default_keys.begin();
	std::map<std::string, Value>::iterator ite = _default_keys.end();
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
	return (SUCCESS);
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
	while (_pos < _size && _content[_pos] != ';' && _content[_pos] != '}') 
	{
		int	dot = _content.find_first_of(";", _pos);
		if (key == "location")
			dot = _content.find_first_of("}", _pos);
		std::string value = _content.substr(_pos, dot - _pos);
		if (!(checkValue(key, value, _server.getRefPorts().back())))
			return (0);
		_pos = dot;
	}
	if (_pos == _size)
		return (0);
	_pos++;
	return (SUCCESS);
}

int	Parser::checkPort(std::string raw_value, Value & new_value)
{
	int port = atoi(raw_value.c_str());
	std::cout << "Port " << port << std::endl;
	if (port < 0 || port > 65535)
		return (0);
	new_value._value = raw_value;
	return (SUCCESS);
}

int	Parser::checkMethod(std::string raw_value, Value & new_value)
{
	std::string	method;
	int			position;

	new_value._value = raw_value;
	while (raw_value.size())
	{
		position = raw_value.find("|");
		if (position < 0)
		{	
			if (raw_value != "GET" && raw_value != "POST" && raw_value != "DELETE")
				return (0);
			break;
		}
		method = raw_value.substr(0, position);
		raw_value = raw_value.substr(position + 1);
		if (method != "GET" && method != "POST" && method != "DELETE")
			return (0);
		new_value._methods.push_back(method);
	}
	return (SUCCESS);
}

int	Parser::checkAutoindex(std::string raw_value, Value & new_value)
{
	if (raw_value != "on" && raw_value != "off")
		return (0);
	new_value._value = raw_value;
	return (SUCCESS);
}

int	Parser::checkClientMaxSize(std::string raw_value, Value & new_value) { (void)raw_value; (void)new_value; return (1); }
int	Parser::checkHost(std::string raw_value, Value & new_value) { (void)raw_value; (void)new_value; return (1);  }
int	Parser::checkServerName(std::string raw_value, Value & new_value) { (void)raw_value; (void)new_value; return (1);  }
int	Parser::checkErrorPage(std::string raw_value, Value & new_value) { (void)raw_value; (void)new_value; return (1);  }
int	Parser::checkRoot(std::string raw_value, Value & new_value) { (void)raw_value; (void)new_value; return (1);  }
int	Parser::checkIndex(std::string raw_value, Value & new_value) { (void)raw_value; (void)new_value; return (1);  }
int	Parser::checkLocation(std::string raw_value, Value & new_value) { (void)raw_value; (void)new_value; return (1);  }

int	Parser::checkValue(std::string key, std::string value, Port & port)
{
	std::map<std::string, Value>::iterator			ite = port.getConfig().end();
	std::map<std::string, Value>::iterator			it = port.getConfig().find(key);
	std::map<std::string, validity_fct>::iterator	cite = _key_checker.end();
	std::map<std::string, validity_fct>::iterator	cit = _key_checker.find(key);
	Value											new_value;

	if (it == ite || cite == cit)
		return (0);
	if (!((this->*_key_checker[key])(value, new_value)))
	{
		std::cout << key << " : " << value << " is not valid" << std::endl;
		return (0);
	}
	_server.getRefPorts().back().getConfig()[key] = Value(value);
	return (1);
}

Server	Parser::getServer( void ) { return _server; }


// int Parser::defaultConfiguration(void)
// {
// 	std::ifstream file("./conf/dictConf.default");
// 	std::string line;
// 	while (std::getline(file, line))
// 	{
// 		int i = 0;
// 		while (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\v')
// 			i++;
// 		if (line[i] == '#')
// 			continue ;
// 		else if (!line[i] || line[i] == '\n')
// 			continue ;
// 		else
// 		{
// 			int dbl = line.find(":");
// 			if (static_cast<int>(dbl) == -1)
// 				return (1);
// 			std::string key = line.substr(i, dbl - i);
// 			std::string value = line.substr(dbl + 1, line.length());
// 			while (static_cast<int>(key.find(" ")) != -1 || static_cast<int>(key.find("\t")) != -1)
// 				key = key.substr(0, key.length() - 1);
// 			int k = 0;
// 			while (value[k] == ' ' || value[k] == '\t' || value[k] == '\r' || value[k] == '\v')
// 				k++;
// 			value = value.substr(k, value.length() - k);
// 			while (static_cast<int>(value.find(" ")) != -1 || static_cast<int>(value.find("\t")) != -1)
// 				value = value.substr(0, value.length() - 1);
// 			_default_key_checker[key] = Value(value);
// 		}
// 	}
// 	return SUCCESS;
// }

}