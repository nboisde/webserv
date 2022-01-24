#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

namespace ws
{
	class Response {
		public:
			static std::map<int, std::string>	_status_code;

			Response( void );
			Response( Response const & src );
			Response( int status );

			virtual ~Response();

			Response &		operator=( Response const & rhs );

			std::string 		genStatusLine( int status );
			std::string			genDate( void );
			std::string			genConnection( void );
			std::string	const &	genHeader( void );

			const char *	response( void ); //CREER LA REPONSE A PARTIR DE ENTETE ET DONNES DE RETOUR
			void			addContentLength( void );
			void			treatCGI( std::string cgi_output ); //DISSEQUE LA REPONSE DU CGI

			std::string		getResponse( void ) const;
			std::string		getStatusLine(void) const;
			std::string		getHeader( void ) const;
			std::string		getBody( void ) const;
			void			setResponse( std::string resp );
			void			setStatusLine( std::string status_l );
			void			setHeader( std::string header );
			void			setBody( std::string newbody );

		private:
			std::string		_response;

			std::string 	_status_line;
			std::string		_header;
			std::string		_body;
			int				_status;
	};
}
#endif
