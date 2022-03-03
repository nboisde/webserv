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
		
		int				executeExtension( Server & serv, Port & port );
		int				executeAutoin( void );
		void			executeRedir( void );
		void			executeHtml( void );
		int				executeError( void );
		int				delete_ressource( void );
		error_type		init_responseMap( void );
		int				uploadAuthorized( void );
		bool			uploadFiles( Server & serv );
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
		std::string		_url;
		std::string		_root;
		Route			_route;
		map_configs		_config;
		error_type		_errors;
		std::string		_hostname;
		std::string		_extension;

		//GUIGS PATCH
		bool			_file_complete;
		bool			_cgi_complete;
		FILE *			_cgi_tmp_file;
		std::string		_cgi_response;
		FILE *			_tmp_file;
		int				_upload_fd;

	public:
		Client( void );
		Client( int fd, struct sockaddr_in *cli_addr, map_configs conf );
		Client( Client const & src );
		virtual ~Client();
			
		Client &		operator=( Client const & rhs );

		bool 			TmpFileCompletion(Server & serv);
		int				execution( Server & serv, Port & port );

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
		std::string		getRoot( void ) const;
		std::string		getUrl( void ) const;

		//GUIGs PATCH
		bool			read_fd_out( Server & serv );
		bool			getFileFlag(void) const;
		void			setFileFlag(bool);
		FILE *			getTmpFile( void ) const;
		int				getUploadFd(void ) const;
		void			resetValues( void );
		bool			getCGIFlag(void) const;
		FILE *			getCGIFile( void ) const;
		void			setCGIFile( FILE * newfd );
		std::string		getCGIResponse( void ) const;

};

}

#endif
