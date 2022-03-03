#ifndef PARSER_HPP
# define PARSER_HPP

# include "webserv.hpp"
# include "Server.hpp"
# include "Value.hpp"
# include "Port.hpp"

namespace ws
{
class Parser
{
		typedef std::vector<Port>::iterator 			it_port;
		typedef	int (Parser::*validity_fct)(std::string, Value &);
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
		checker_type		_default_loc_keys;

		void				initParser(void);

		int					checkFileName(void);
		int					readFile(void);
		int					initWebServer(void);
		int					checkHttp(void);
		int					checkServer(void);
		int 				checkLine(std::string value, int nb_of_elem);
		int					checkKeys(keys_type & new_config);
		int					setValues(std::string key, keys_type & new_config);
		int					checkValue(std::string key, std::string value, keys_type & new_config);

		int					checkRouteKeys( Route & route );
		int					setRouteValues( std::string * value, std::vector<std::string> * methods );
		int					checkRouteMethod( std::string new_value, std::vector<std::string> * methods);

		int					checkPort(std::string, Value &);
		int					checkMethod(std::string, Value &);
		int					checkAutoindex(std::string, Value &);	
		int					checkClientMaxSize(std::string, Value &);
		int					checkServerName(std::string, Value &);
		int					checkErrorPage(std::string, Value &);
		int					checkRoot(std::string, Value &);
		int					checkIndex(std::string, Value &);
		int					checkLocation(std::string, Value &);
		int					checkUpload(std::string, Value &);
		int					checkReturn(std::string, Value &);
		int 				checkCGI(std::string, Value &);

	public:
		
		Parser();
		virtual ~Parser();

		Server				getServer(void);
		int					launch(std::string file);
};
}

#endif
