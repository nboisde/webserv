#include "parsing.hpp"

int		checkFileName(char *file)
{
	std::string s_file(file);
	std::cout << s_file << std::endl;
	return (1);
}

int		readFile(char *file, std::string *content)
{
	(void)file;
	(void)content;
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
