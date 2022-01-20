#ifndef __REQUEST_HPP__
# define __REQUEST_HPP__

# include "webserv.hpp"

namespace ws 
{

class Request
{
	typedef std::map<std::string, std::string> head_type;

	public:
		Request( void );
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

		
		// GETTERS
		std::string		getRawContent(void) const;
		int				getMethodType(void) const;
		int				getContentLength(void) const;
		int				getHeaderSizeRevieved(void) const;
		int				getBodySizeReceived(void) const;
		int				getHeaderSize(void) const;
		std::string		getHeader(void) const;
		std::string		getBody(void) const;
		int				getState( void ) const;
		head_type		getHead( void ) const;
		head_type &		getHead( void );

	private:
		int			findProtocol(std::string buf);
		int			errorHandling(std::vector<std::string> v, int i);
		int 		errorReturn(void);
		int			bodyReceived(void);

		int							_line;
		int							_cursor;
		int							_state;
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
};

}
#endif
