#include "Parser.hpp"

namespace ws
{

Parser::Parser(void) : _server("127.0.0.1")
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

int	Parser::checkFileName()
{
	int size = _config_file.size();
	int pos = _config_file.rfind(".conf");
	if (pos == -1)
		return (0);
	else if ((size - pos) != 5)
		return (0);
	return (1);
}

int	Parser::readFile()
{
	std::ifstream	ifs(file);
	std::string		buffer;

	while (getline(ifs, buffer))
	{
		*content += buffer;
		*content += "\n";
	}
	ifs.close();
	return (1);
}

int		checkHttp(std::string content, int i, int size)
{
	int pos;

	while (i < size && isspace(content[i]))
		i++;
	pos = content.find("http");
	if (pos != i)
		return (0);
	i = 4;
	while (i < size && isspace(content[i]))
		i++;
	if (content[i] != '{')
		return (0);
	return (i);
}


int		checkInfo(std::string content, int i, int size)
{
	while (i < size)
	{

		i = checkElem(content, i, size);
		if (!i)
			return (0);
		while (i < size && isspace(content[i]))
			i++;
		if (content[i] == ';')
			break;
	}
	if (i == size && content[i] != ';')
		return (0);
	return (i);
}

int		checkServer(std::string content, int i, int size)
{
	int	pos;

	while (i < size && isspace(content[i]))
		i++;
	pos = content.find("server", i);
	if (pos != i)
		return (0);
	i += 6;
	while (i < size)
	{
		i = checkInfo(content, i, size);
		if (!i)
			return (0);
		while (i < size && isspace(content[i]))
			i++;
		if (content[i] == '}')
			break;
	}
	if (i == size && content[i] != '}')
		return (0);
	return (i);
}

int		initWebServer(std::string content, ws::Server *server)
{
	int	size = content.size();
	int	pos = 0;

	(void)server;
	if (!(pos = checkHttp(content, pos, size)))
		return (0);
	while (pos < size)
	{
		if (!(pos = checkServer(content, pos, size)))
			return (0);
		pos++;
	}
	return (1);
}



}