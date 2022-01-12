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
		Server						_server;
		std::string					_config_file;
		std::string					_content;
		int							_size;
		int							_pos;
		std::vector<std::string>	_keys;

		int		checkFileName(void);
		int		readFile(void);
		int		initWebServer(void);
		int		checkHttp(void);
		int		checkServer(void);
		int		checkContent(void);
		int		checkKeyValue(void);

	public:
		Parser();
		virtual ~Parser();

		int		launch(std::string file);
};s
}

#endif
