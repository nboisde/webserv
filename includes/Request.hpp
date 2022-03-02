#ifndef __REQUEST_HPP__
# define __REQUEST_HPP__

# include "webserv.hpp"
# include "Utils.hpp"

namespace ws 
{

class Request
{
	typedef std::map<std::string, std::string> head_type;

	public:
		Request( void );
		Request( int up_auth );
		Request( Request const & src );
		virtual ~Request( void );

		Request &		operator=( Request const & rhs );

		int				concatenateRequest(std::string tmp);
		int				requestReceptionState();
		int				identifyBodyLengthInHeader(void);
		int				isTransferEncoding(void) const;
		int				checkHeaderEnd() const;
		void			findMethod(void);
		int				fillHeaderAndBody(void);
		void			ChunkedBodyProcessing(std::string body);
		int				parseHeader(void);
		void			manageConnection( std::string str );
		void			resetValues(void);
		int				multipartForm( void );
		int				findContinue( void );
		int				multipartFormRaw( void );
		int				analyseURL(void);
		void			cleanRawContent(void);


		// GETTERS
		std::string		getRawContent(void) const;
		int				getMethodType(void) const;
		int				getContentLength(void) const;
		int				getHeaderSizeRevieved(void) const;
		int				getBodySizeReceived(void) const;
		int				getHeaderSize(void) const;
		std::string		getHeader(void) const;
		std::string	&	getBody(void);
		std::string		getBody(void) const;
		void			setBody(std::string);
		int				getState( void ) const;
		head_type		getHead( void ) const;
		head_type &		getHead( void );
		int				getConnection( void ) const;
		int				getStatus( void ) const;
		int				getMultipart( void ) const;
		std::string		getBoundary( void ) const;
		int				getContinue( void ) const;
		int				getUploadAuthorized( void ) const;

		void			setContinue( int cont );
		void			setUploadAuthorized( int up_auth );
		void			setHeadKey( std::string key, std::string value );


	private:
		int			findProtocol(std::string buf);
		int			errorHandling(std::vector<std::string> v, int i);
		int 		errorReturn( int opt );
		int			bodyReceived(void);
		int			findMultiEnd( void );
		int			findBodyEnd( void );

		int							_line;
		int							_cursor;
		int							_state;
		int							_connection;
		std::string					_raw_content;
		int							_body_reception_encoding;
		int							_body_len_received;
		int							_header_len_received;
		int							_content_length;
		int							_method_type;
		int 						_header_size;
		std::string					_header;
		std::string					_body;
		std::vector<std::string>	_vheader;
		head_type					_head;
		int							_status;
		int							_multipart;
		std::string					_boundary;
		int							_continue;
		int							_upload_authorized;
};

}
#endif
