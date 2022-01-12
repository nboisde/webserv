#include "parsing.hpp"
#include <fstream>

int		checkFileName(char *file)
{
	int	pos;
	int	size;

	std::string s_file(file);
	size = s_file.size();
	pos = s_file.rfind(".conf");
	if (pos == -1)
		return (0);
	else if ((size - pos) != 5)
		return (0);
	return (1);
}

int		readFile(char *file, std::string *content)
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

int		checkHttpBegin(std::string content, int i, int size)
{
	while (i < size && isspace(content[i]))
		i++;
	i = (&(content[i])).find("http");
	if (i != 0)
		return (0);
	i = 4;
	while (i < size && isspace(content[i]))
		i++;
	if (content[i] != '{')
		return (0);
	return (i);
}

int		checkHttpEnd(std::string content, int pos, int size)
{
	while (pos < size && isspace(content[pos]))
		pos++;
	if (content[pos] != '}')
		return (0);
	while (pos < size && isspace(content[pos]))
		pos++;
	if (pos != size)
		return (0);
	return (pos);
}

int		checkServer(std::string content, int i, int size)
{
	int	pos;

	while (i < size && isspace(content[i]))
		i++;
	pos = (&(content[i])).find("server");
	if (pos != 0)
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

int		initServer(std::string content, ws::Server *server)
{
	int	size = content.size();
	int	pos = 0;

	(void)server;
	if (!(pos = checkHttpBegin(content, pos, size)))
		return (0);
	while (pos < size)
	{
		if (!(pos = checkServer(content, pos, size)))
			return (0);
		pos++;
	}
	//if ((!checkHttpEnd(content, pos, size)))
	//	return (0);
	return (1);
}

int	parsing(char *file, ws::Server *server)
{
	std::string content;

	if (!checkFileName(file))
		return (0);
	if (!readFile(file, &content))
		return (0);
	if (!initServer(content, server))
		return (0);
	return (1);
}
