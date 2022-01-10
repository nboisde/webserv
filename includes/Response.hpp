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
			virtual ~Response();

			Response &		operator=( Response const & rhs );

			std::string 	genStatusLine( void );


			char*			response( void ); //CREER LA REPONSE A PARTIR DE ENTETE ET DONNES DE RETOUR
			size_t			response_size( void ); //RENVOIE LA TAILLE DE LA REP

			std::string		getResponse( void ) const;
			std::string		getDate( void ) const;
			std::string		getStatusLine(void) const;
			void			setResponse( void );
			void			setDate( void );
			void			setStatusLine(void);
	
		private:
			std::string 	_status_line;
			std::string		_date;
            std::string		_response;
	};
}
#endif
