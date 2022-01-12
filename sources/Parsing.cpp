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
		*content += buffer;
	ifs.close();
	return (1);
}

int		initServer(std::string content, ws::Server *server)
{
	(void)content;
	(void)server;
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
