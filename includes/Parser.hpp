#ifndef PARSER_HPP
# define PARSER_HPP

# include "webserv.hpp"
# include "Server.hpp"
# include "Value.hpp"

namespace ws
{
class Parser
{
		typedef	int (Parser::*validity_fct)(std::string);
		typedef std::map<std::string, validity_fct>		checker_type;
		typedef	std::map<std::string, Value>			keys_type;

	private:

		int					_size;
		int					_pos;
		std::string			_config_file;
		std::string			_content;
		Server				_server;
		checker_type		_key_checker;
		keys_type			_default_keys;

		int					checkFileName(void);
		int					readFile(void);
		int					initWebServer(void);
		int					checkHttp(void);
		int					checkServer(void);
		int					checkKeys(void);
		int					setValues(std::string key);
		int					checkValue(std::string key, std::string value, Port & port);

		int					checkPort(std::string value);
		int					checkMethod(std::string value);
		int					checkAutoindex(std::string value);	
		int					checkClientMaxSize(std::string value);
		int					checkHost(std::string value);
		int					checkServerName(std::string value);
		int					checkErrorPage(std::string value);
		int					checkRoot(std::string value);
		int					checkIndex(std::string value);
		int					checkLocation(std::string value);

	public:
		
		Parser();
		virtual ~Parser();

		Server				getServer(void);
		int					launch(std::string file);
};
}

#endif
