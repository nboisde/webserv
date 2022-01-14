#include "Parser.hpp"

namespace ws
{

Parser::Parser(void) : _server("127.0.0.1"), _pos(0)
{
	_keys.push_back("listen");
	_keys.push_back("server_name");
	_keys.push_back("client_max_body_size");
	_keys.push_back("error_page");
	_keys.push_back("autoindex");
}

Parser::~Parser(void)
{
}

Server	Parser::getServer(void)
{
	return _server;
}


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
	_server.addPort(Port());
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
	int	key_index;
	int key_total = _keys.size();

	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	for (key_index = 0; key_index < key_total; key_index++)
	{
		int	i = _content.find(_keys[key_index], _pos);
		if (i == _pos)
		{
			_pos += _keys[key_index].size();
			break;
		}
	}
	if (key_index == key_total)
		return (0);
	if (!checkValues(key_index))
		return (0);
	return (1);
}

int	Parser::checkValues(int key)
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
		setValue(key, value);
		_pos = dot;
	}
	if (_pos == _size)
		return (0);
	_pos++;
	return (1);
}

int	Parser::setValue(int key, std::string value)
{
	if (key == 0)
		_server.getRefPorts().back().setPort(strtol(value.c_str(), NULL, 10));
	else if (key == 1)
		_server.getRefPorts().back().setServerName(value);
	else if (key == 2)
		_server.getRefPorts().back().setClientMaxSize(strtol(value.c_str(), NULL, 10));
	else if (key == 4)
		_server.getRefPorts().back().setAutoindex(1);
	else 
		return (0);
	return (1);
}
}
