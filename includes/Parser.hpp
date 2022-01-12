#ifndef PARSER_HPP
# define PARSER_HPP

# include "webserv.hpp"
# include "Server.hpp"
# include <utility>
# include <fstream>

namespace ws
{
class Parser
{
	private:
		Server							_server;
		std::string						_content;
		int								_size;
		int								_pos;
		std::vector<std::string>		_keys;

		int		checkFileName(char *file);
		int		readFile(char *file, std::string *content);
		int		initWebServer(std::string content, ws::Server *server);
		int		checkHttp(std::string content, int i, int size);
		int		checkServer(std::string content, int i, int size);
		int		checkContent(std::string content, int i, int size);
		int		checkKeyValue(std::string content, int i, int size);

	public:
		Parser();
		virtual ~Parser();

		int		launch(char *file, ws::Server *server);
};
}

#endif
