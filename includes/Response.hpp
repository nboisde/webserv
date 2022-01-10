#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

namespace ws
{
	class Client;

	class Response {
		public:
			static std::map<int, std::string>	_status_code;

			Response( void );
			Response( Response const & src );
			virtual ~Response();

			Response &		operator=( Response const & rhs );

			std::string 	genStatusLine( void );
			std::string		genHeader( void );

			const char *	response( void ); //CREER LA REPONSE A PARTIR DE ENTETE ET DONNES DE RETOUR
			size_t			response_size( void ); //RENVOIE LA TAILLE DE LA REP

			std::string		getResponse( void ) const;
			std::string		getHeader( void ) const;
			std::string		getStatusLine(void) const;
			void			setResponse( std::string resp );
			void			setHeader( std::string header );
			void			setStatusLine( std::string status_l );
	
		private:
			std::string 	_status_line;
			std::string		_header;
			std::string		_content;
            std::string		_response;
	};
}
#endif
