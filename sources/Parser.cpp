#include "Parser.hpp"

namespace ws
{

Parser::Parser(void) : _pos(0), _server("127.0.0.1") { initParser(); }
Parser::~Parser(void) {}

Server	Parser::getServer( void ) { return _server; }

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
	}
	if (it == ite)
	{
		new_config["listen"]._default = true;
		_server.addPort(Port(new_config["server_name"]._value, new_config));
	}
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
	if (!setValues((*it).first, new_config))
		return (0);
	return (SUCCESS);
}

int	Parser::setValues(std::string key, keys_type & new_config)
{
	int	isspaceNb = 0;
	int	grammar;

	while (_pos < _size && isspace(_content[_pos]))
	{
		_pos++;
		isspaceNb++;
	}
	if (!isspaceNb)
		return (0);
	if (key == "location")
		grammar = _content.find_first_of("{", _pos);
	else 
		grammar = _content.find_first_of(";", _pos);
	if (grammar == -1)
		return (0);
	std::string value = _content.substr(_pos, grammar - _pos);
	if (!(checkValue(key, value, new_config)))
		return (0);
	if (key != "location")
		_pos = grammar + 1;
	return (SUCCESS);
}

int	Parser::checkValue(std::string key, std::string value, keys_type & new_config)
{
	std::map<std::string, Value>::iterator			ite = new_config.end();
	std::map<std::string, Value>::iterator			it = new_config.find(key);
	std::map<std::string, validity_fct>::iterator	cite = _key_checker.end();
	std::map<std::string, validity_fct>::iterator	cit = _key_checker.find(key);

	if (it == ite || cite == cit)
		return (0);
	if (value.empty() || !((this->*_key_checker[key])(value, _default_keys[key])))
	{
		std::cout << key << " : " << value << " is not valid" << std::endl;
		return (0);
	}
	new_config[key] = _default_keys[key];
	return (SUCCESS);
}

int Parser::checkLine(std::string value, int nb_of_elem)
{
	int i = 0;
	int nb = 0;

	while (value[i])
	{
		int start = i;
		while (!isspace(value[i]) && value[i])
			i++;
		if (start != i)
			nb++;
		while (isspace(value[i]))
			i++;
	}
	if (nb_of_elem != nb)
		return 0;
	else
		return 1;
}

int	Parser::checkPort(std::string raw_value, Value & new_value)
{
	if (!checkLine(raw_value, 1))
		return 0;
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

	if (!checkLine(raw_value, 1))
		return 0;
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
	if (!checkLine(raw_value, 1))
		return 0;
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
int	Parser::checkServerName(std::string raw_value, Value & new_value) 
{ 
	if (!checkLine(raw_value, 1))
		return 0;
	new_value._value = raw_value; return (1); 
}
int	Parser::checkErrorPage(std::string raw_value, Value & new_value) 
{
	if (!checkLine(raw_value, 2))
		return 0;
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
int	Parser::checkRoot(std::string raw_value, Value & new_value) 
{ 
	if (!checkLine(raw_value, 1))
		return 0;
	new_value._value = raw_value; return (1); 
}
int	Parser::checkIndex(std::string raw_value, Value & new_value)
{
	if (!checkLine(raw_value, 1))
		return 0;
	new_value._value = raw_value; return (1);
}
int	Parser::checkUpload(std::string raw_value, Value & new_value)
{
	if (!checkLine(raw_value, 1))
		return 0;
	new_value._value = raw_value; return (1);  
}
int	Parser::checkLocation(std::string raw_value, Value & new_value) 
{
	size_t length = 0;
	while (!isspace(raw_value[length]))
		length++;
	if (raw_value.size() > length)
		raw_value = raw_value.substr(0, length);
	new_value._value += raw_value;
	_pos += raw_value.size();
	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	if (_content[_pos] != '{')
		return (0);
	_pos++;

	std::pair<std::string, Route> new_path(raw_value, Route());
	new_value._locations.insert(new_path);
	new_value._locations[raw_value].route = raw_value;
	while (_pos < _size && _content[_pos] != '}')
	{
		while (_pos < _size && isspace(_content[_pos]))
			_pos++;
		if (!(checkRouteKeys(new_value._locations[raw_value])))
			return (0);
		while (_pos < _size && isspace(_content[_pos]))
			_pos++;
	}
	if (_content[_pos] != '}')
		return (0);
	_pos++;
	return (SUCCESS);
}

int	Parser::checkRouteKeys( Route & route )
{
	int key;

	if ((key = _content.find("rewrite", _pos)) == _pos)
	{
		_pos += 7;
		if (!(setRouteValues(&(route.rewrite), 0)))
			return (0);
	}
	else if ((key = _content.find("autoindex", _pos)) == _pos)
	{
		_pos += 9;	
		if (!(setRouteValues(&(route.autoindex), 0)))
			return (0);
	}
	else if ((key = _content.find("index", _pos)) == _pos)
	{
		_pos += 5;
		if (!(setRouteValues(&(route.index), 0)))
			return (0);
	}
	else if ((key = _content.find("upload", _pos)) == _pos)
	{
		_pos += 6;
		if (!(setRouteValues(&(route.upload), 0)))
			return (0);
	}
	else if ((key = _content.find("method", _pos)) == _pos)
	{
		_pos += 6;
		if (!(setRouteValues(0, &(route.methods))))
			return (0);
	}
	else
		return (0);
	return (SUCCESS);
}

int	Parser::setRouteValues(std::string * value, std::vector<std::string> * methods)
{
	int			isspaceNb = 0;
	int			grammar;
	std::string	new_value;

	while (_pos < _size && isspace(_content[_pos]))
	{
		_pos++;
		isspaceNb++;
	}
	if (!isspaceNb)
		return (0);
	if ((grammar = _content.find_first_of(";", _pos)) == -1)
		return (0);
	new_value = _content.substr(_pos, grammar - _pos);
	if (!checkLine(new_value, 1))
		return (0);
	if (value)
		*value = new_value;
	else
		if (!(checkRouteMethod(new_value, methods)))
			return (0);
	_pos = grammar + 1;
	return (SUCCESS);
}

int	Parser::checkRouteMethod( std::string new_value, std::vector<std::string> * methods )
{
	std::string	method;
	int			position;

	methods->clear();
	while (new_value.size())
	{
		position = new_value.find("|");
		if (position < 0)
		{	
			if (new_value != "GET" && new_value != "POST" && new_value != "DELETE")
				return (0);
			methods->push_back(new_value);
			break;
		}
		method = new_value.substr(0, position);
		new_value = new_value.substr(position + 1);
		if (method != "GET" && method != "POST" && method != "DELETE")
			return (0);
		methods->push_back(method);
	}
	return (SUCCESS);
}

static bool microparse_extension(std::string extension)
{
	if (*(extension.begin()) != '.')
		return false;
	for (int pos = 1; extension[pos]; pos++)
	{
		if (!isalnum(extension[pos]) || !islower(extension[pos]))
			return false;
	}
	return true;
}

//IN CHECKCGI WE CREATE A SINGLE INSTANCE OF VALUE NAMED CGI
//This Value store a pair in _list, and the raw string in _value. Nothing else is used.
//the pair is composed of :
// 		1) the Extension encountered ".php", ".mp3", ".py", etc...
//		2) the Path to the binary, that launch that extension
int Parser::checkCGI(std::string raw_value, Value & cgi)
{
	int i;

	if (!cgi._value.empty())
		cgi._value += " | ";
	cgi._value += raw_value;
	for(i = 0; raw_value[i] && !isspace(raw_value[i]); i++);

	//HERE WE PARSE THE EXTENSION, AND SAVE IT IN A STRING
	std::string extension = raw_value.substr(0, i);
	
	for(; raw_value[i] && isspace(raw_value[i]); i++);
	int start = i;
	for(; raw_value[i] && !isspace(raw_value[i]); i++);

	//HERE WE PARSE THE BINARY PATH, AND SAVE IT IN A STRING
	std::string binary = raw_value.substr(start, i - start);

	for(; raw_value[i] && isspace(raw_value[i]); i++);
	if (raw_value[i] || binary.empty() || !microparse_extension(extension) || !isfile(binary))
		return (0);
	cgi._list.insert(std::pair<std::string, std::string>(extension, binary));
	return SUCCESS;
}

void	Parser::initParser(void)
{
	_key_checker["listen"] = &Parser::checkPort;
	_key_checker["server_name"] = &Parser::checkServerName;
	_key_checker["client_max_body_size"] = &Parser::checkClientMaxSize;
	_key_checker["error_page"] = &Parser::checkErrorPage;
	_key_checker["autoindex"] = &Parser::checkAutoindex;
	_key_checker["method"] = &Parser::checkMethod;
	_key_checker["root"] = &Parser::checkRoot;
	_key_checker["index"] = &Parser::checkIndex;
	_key_checker["location"] = &Parser::checkLocation;
	_key_checker["upload"] = &Parser::checkUpload;
	_key_checker["cgi"] = &Parser::checkCGI;
	_default_keys["listen"] = Value("8080");
	_default_keys["upload"] = Value("");
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
	_default_keys["index"] = Value("");
	_default_keys["location"] = Value("");
	_default_keys["cgi"] = Value("");
}


}
