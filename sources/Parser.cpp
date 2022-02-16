#include "Parser.hpp"

namespace ws
{

Parser::Parser(void) : _pos(0), _server("127.0.0.1") { initParser(); }
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
	keys_type new_config(_default_keys);
	while (_pos < _size)
	{
		if (!checkKeys(new_config))
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
	it_port it = _server.getRefPorts().begin();
	it_port ite = _server.getRefPorts().end();
	for (; it != ite; it++)
	{
		std::string first_config_port = (*(it->getConfig()).begin()).second["listen"]._value;
		if (first_config_port == new_config["listen"]._value)
		{
			it->getConfig()[new_config["server_name"]._value] = new_config;
			break;
		}
		std::cout << first_config_port << std::endl;
		std::cout << new_config["listen"]._value << std::endl;
	}
	if (it == ite)
		_server.addPort(Port(new_config["server_name"]._value, new_config));
	return (SUCCESS);
}


int	Parser::checkKeys(keys_type & new_config)
{
	std::map<std::string, Value>::iterator it = new_config.begin();
	std::map<std::string, Value>::iterator ite = new_config.end();
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
	std::cout << "FOUND KEY " << it->first << std::endl;
	if (it->first == "location")
	{
		if (!checkLocationKeys(new_config["location"]))
			return (0);
	}
	else 
	{	
		if (!setValues((*it).first, new_config, 0))
			return (0);
	}
	return (SUCCESS);
}

int	Parser::checkLocationKeys(Value & loc_config, keys_type & new_config)
{
	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	int start = _pos;
	while (_pos < _size && !isspace(_content[_pos]) && _content[_pos] != '{')
		_pos++;
	int end = _pos - start;
	std::string path = _content.substr(start, end);
	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	if (_content[_pos] != '{')
		return (0);
	_pos++;
	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	std::pair<std::string, route> new_path(path, route());
	loc_config._locations.insert(new_path);

	std::vector<std::string>::iterator it = loc_config._locations[path].keys.begin();
	std::vector<std::string>::iterator ite = loc_config._locations[path].keys.end();
	
	int	found = 0;
	for (; it != ite; it++)
	{
		std::cout << "KEY " << *it << std::endl;
		int i = _content.find(*it, _pos);
		std::cout << "POS " << _pos << std::endl;
		std::cout << "FIND " << i << std::endl;
		if (i == _pos)
		{
			found = 1;
			_pos += it->size();
			break;
		}
	}
	std::cout << "FOUND KEY " << *it << std::endl;
	return (0);
	if (!found)
		return (0);
	if (!setValues(*it, new_config, &(loc_config._locations[path]), 1))
		return (0);
	return (SUCCESS);
}


int	Parser::setValues(std::string key, keys_type * new_config, route * location)
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
		if (dot == -1)
			return (0);
		std::string value = _content.substr(_pos, dot - _pos);
		if (!(checkValue(key, value, new_config)))
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

	if (port < 0 || port > 65535)
		return (0);
	new_value._value = raw_value;
	return (SUCCESS);
}

int	Parser::checkMethod(std::string raw_value, Value & new_value)
{
	std::string	method;
	int			position;

	new_value._methods.clear();
	new_value._value = raw_value;
	while (raw_value.size())
	{
		position = raw_value.find("|");
		if (position < 0)
		{	
			if (raw_value != "GET" && raw_value != "POST" && raw_value != "DELETE")
				return (0);
			new_value._methods.push_back(raw_value);
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

int	Parser::checkClientMaxSize(std::string raw_value, Value & new_value)
{
	int	i = 0;
	int	size = raw_value.size();
	int	body_size = atoi(raw_value.c_str());

	for (; i < size; i++)
		if (!::isdigit(raw_value[i]))
			break;
	if (i != size)
	{
		if (raw_value[i] == 'G' && i == size - 1)
			body_size *= GIGA;
		else if (raw_value[i] == 'M' && i == size - 1)
			body_size *= MEGA;
		else if (raw_value[i] == 'K' && i == size - 1)
			body_size *= KILO;
		else
			return (0);
	}
	new_value._value = raw_value;
	new_value._max_body_size = body_size;

	return (SUCCESS);
}
int	Parser::checkHost(std::string raw_value, Value & new_value) { new_value._value = raw_value; return (1);  }
int	Parser::checkServerName(std::string raw_value, Value & new_value) { new_value._value = raw_value; return (1);  }
int	Parser::checkErrorPage(std::string raw_value, Value & new_value) 
{
	int			size = raw_value.size();
	std::string	str_error;
	int			nbr_error;
	int			pos = 0;

	new_value._value += raw_value;
	for (; pos < size && !isspace(raw_value[pos]); pos++) {}
	str_error = raw_value.substr(0, pos);
	nbr_error = atoi(raw_value.c_str());
	if (nbr_error < 100 || nbr_error > 600)
		return (0);
	for (; pos < size && isspace(raw_value[pos]); pos++) {}
	str_error = raw_value.substr(pos);
	for (; pos < size && !isspace(raw_value[pos]); pos++) {}
	if (pos != size)
		return (0);
	new_value._errors.insert(std::pair<int, std::string>(nbr_error, str_error));
	return (SUCCESS);  
}
int	Parser::checkRoot(std::string raw_value, Value & new_value) { new_value._value = raw_value; return (1);  }
int	Parser::checkIndex(std::string raw_value, Value & new_value) { new_value._value = raw_value; return (1);  }
int	Parser::checkUpload(std::string raw_value, Value & new_value) { new_value._value = raw_value; return (1);  }
int	Parser::checkReturn(std::string raw_value, Value & new_value) { new_value._value = raw_value; return (1);  }
int	Parser::checkLocation(std::string raw_value, Value & new_value) 
{
	int			pos = 0;
	int			size = raw_value.size();
	std::string	location;
	std::string	substitute;

	new_value._value += raw_value;
	for (; pos < size && !isspace(raw_value[pos]); pos++) {}
	location = raw_value.substr(0, pos);
	for (; pos < size && isspace(raw_value[pos]); pos++) {}
	substitute = raw_value.substr(pos);
	for (; pos < size && !isspace(raw_value[pos]); pos++) {}
	if (pos != size)
		return (0);
	//new_value._locations.insert(std::pair<std::string, std::string>(location, substitute));
	return (SUCCESS);  
}

int	Parser::checkValue(std::string key, std::string value, keys_type * new_config, route * location)
{
	std::map<std::string, Value>::iterator			ite = new_config->end();
	std::map<std::string, Value>::iterator			it = new_config->find(key);
	std::map<std::string, validity_fct>::iterator	cite = _key_checker.end();
	std::map<std::string, validity_fct>::iterator	cit = _key_checker.find(key);

	if (it == ite || cite == cit)
		return (0);
	if (!((this->*_key_checker[key])(value, _default_keys[key], )))
	{
		std::cout << key << " : " << value << " is not valid" << std::endl;
		return (0);
	}
	new_config[key] = _default_keys[key];
	return (SUCCESS);
}

void	Parser::initParser(void)
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
	_key_checker["upload"] = &Parser::checkUpload;
	_key_checker["return"] = &Parser::checkReturn;
	_default_keys["listen"] = Value("8080");
	_default_keys["host"] = Value(LOCALHOST);
	_default_keys["server_name"] = Value(LOCALHOST);
	_default_keys["client_max_body_size"] = Value("100000000");
	_default_keys["error_page"] = Value("");
	_default_keys["autoindex"] = Value("off");
	_default_keys["method"] = Value("GET");
	_default_keys["method"]._methods.push_back("GET");
	char *buf = NULL;
	buf = getcwd(buf, 0);
	std::stringstream stream;
	stream << buf << "/www";
	_default_keys["root"] = Value(stream.str());
	free(buf);
	_default_keys["index"] = Value("/html/index.html");
	_default_keys["location"] = Value("");
}

Server	Parser::getServer( void ) { return _server; }

}
