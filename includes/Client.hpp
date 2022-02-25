#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include "webserv.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "CGI.hpp"
# include "Server.hpp"
# include "Port.hpp"
# include "Value.hpp"
# include "Listdir.hpp"

namespace ws {

class Port;

class Client
{
	typedef	std::map<std::string, Value>		config_type;
	typedef std::map<int, std::string>			error_type;
	typedef std::map<std::string, config_type>	map_configs;


	private:
		void			saveLogs( void );
	
		void			setPath( void );
		void			setRoute( void );
		int				setExecution( void );
		int				setHostname( void );
	
		int				checkCGI( void );
		int				checkPath( void );
		int				checkRedirection( void );
		int				checkAutoindex( void );
		int				checkUpload( void );
		int				checkLocation( std::string & url, std::string & route);
		int				checkMethod( void );
		int				checkExtension( std::string & root, std::string & url );
		
		int				executeExtension( Server const & serv, Port & port );
		int				executeAutoin( void );
		void			executeRedir( void );
		void			executeHtml( void );
		int				executeError( void );
	
		error_type		init_responseMap( void );
		int				uploadAuthorized( void );
		int				uploadFiles( std::string upload_path );
		int				isURLDirectory( void );
		int				directoryProcessing( std::string url );


	protected:
		int				_fd;
		int				_status;
		std::string		_ip;
		std::string		_port;
		ws::Request		_req;
		ws::Response	_res;
		std::string		_file_path;
		Route			_route;
		map_configs		_config;
		error_type		_errors;
		std::string		_hostname;
		std::string		_extension;


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
		map_configs const & getConfig( void );
		std::string		getHostname( void ) const;
		std::string		getLocalHostname( void) const ;
		std::string const & getExtension( void ) const;

};

}

#endif
