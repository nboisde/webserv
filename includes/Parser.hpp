#ifndef PARSER_HPP
# define PARSER_HPP

# include "webserv.hpp"
# include "Server.hpp"
# include <utility>
# include <fstream>
# include <cstdlib>

namespace ws
{
class Parser
{
		typedef std::map<std::string, std::string>	dictionnary_type;

	private:

		int					_size;
		int					_pos;
		std::string			_config_file;
		std::string			_content;
		Server				_server;
		dictionnary_type	_dict;

		int					checkFileName(void);
		int					readFile(void);
		int					initWebServer(void);
		int					checkHttp(void);
		int					checkServer(void);
		int					checkKeys(void);
		int					checkValues(std::string key);
		int					setValue(std::string key, std::string value, Port & port);
		int					defaultConfiguration(void);

	public:
		
		Parser();
		virtual ~Parser();

		Server				getServer(void);
		int					launch(std::string file);
};
}

#endif
