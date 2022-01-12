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
	std::ifstream	ifs(_config_file);
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
			break;
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
	while (_pos < _size)
	{
		if (!checkKeys())
			return (0);
		while (_pos < _size && isspace(_content[_pos]))
			_pos++;
		if (_content[_pos] == '}')
			break;
	}
	if (_pos == _size)
		return (0);
	return (i);
}


int	Parser::checkKeys(void)
{
	int	key_index;
	int key_total = _keys.size();

	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	for (key_index = 0; key_index <key_total; key_index++)
	{
		int	i = _content.find(_keys[key_index], _pos);
		if (i != _pos)
			break;
	}
	if (key_index == key_total)
		return (0);
	return (1);
}
}
