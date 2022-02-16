#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include "webserv.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "CGI.hpp"
# include "Server.hpp"
# include "Port.hpp"
# include "Value.hpp"

namespace ws {

class Port;

class Client
{
	typedef	std::map<std::string, Value>		config_type;
	typedef std::map<int, std::string>			error_type;
	typedef std::map<std::string, config_type>	map_configs;


	private:
		void			saveLogs(void);
		int				openFile( std::string path );
		int				checkURI( std::string url );
		int				checkCGI( std::string & url );
		int				checkPath( std::string & root, std::string & url );
		int				checkExtension( std::string & root, std::string & url );
		int				executePhpPython( Server const & serv, Port & port, int extension_type );
		int				executeHtml( void );
		int				executeError( void );
		error_type		init_responseMap( void );
		int				uploadAuthorized( void );

	protected:
		int				_fd;
		int				_status;
		std::string		_ip;
		std::string		_port;
		ws::Request		_req;
		ws::Response	_res;
		std::string		_file_path;
		map_configs		_config;
		error_type		_errors;
		std::string		_hostname;


	public:
		Client( void );
		Client( int fd, struct sockaddr_in *cli_addr, map_configs conf );
		Client( Client const & src );
		virtual ~Client();
			
		Client &		operator=( Client const & rhs );

		int				execution( Server const & serv, Port & port );

		int 			receive( void );
		int				send( void );
		void			closeConnection( void );
		void 			bridgeParsingRequest( void );
		int				uploadFiles( void );
		std::string		uploadPath( std::string url );

		int				getFd( void ) const;
		int				getStatus( void ) const;
		ws::Request 	getReq( void ) const ;
		ws::Response	getRes(void ) const;
		ws::Request &	getReq( void );
		ws::Response &	getRes( void );
		std::string		getFilePath( void ) const;
		std::string		getIp( void ) const;
		std::string		getPort( void ) const;
		map_configs		getConfig( void ) const;
		std::string		getHostname( void ) const;

};

}

#endif
