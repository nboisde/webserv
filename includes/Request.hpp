#ifndef __REQUEST_HPP__
# define __REQUEST_HPP__

# include "webserv.hpp"

namespace ws {

class Client;

class Request {
	public:
		Request( void );
		Request( Request const & src );
		virtual ~Request( void );

		Request &	operator=( Request const & rhs );

		int			concatenateRequest(std::string buf);
		int			requestReceptionState();
		void		identifyBodyLengthInHeader(void);
		int			checkHeaderEnd() const;
		void		findMethod(void);
		int			fillHeaderAndBody(void);
		
		// GETTERS
		std::string	getRawContent(void) const;
		int			getMethodType(void) const;
		int			getContentLength(void) const;
		int			getHeaderSizeRevieved(void) const;
		int			getBodySizeRecieved(void) const;
		int			getHeaderSize(void) const;
		std::string	getHeader(void) const;
		std::string	getBody(void) const;
		int			getState( void ) const ;

	private:
		int			_state;
		std::string	_raw_content;
		int			_body_len_recieved;
		int			_header_len_recieved;
		int			_content_length;
		int			_method_type;
		int 		_header_size;
		std::string	_header;
		std::string	_body;
};

}

#endif