#ifndef REQUESTS_HPP
# define REQUESTS_HPP

# include <string>
# include <iostream>

// State MACROS
# define REQUEST_FORMAT_ERROR -1
# define RECIEVING_HEADER 0
# define HEADER_RECIEVED 1
# define BODY_RECIEVED 2

// Methods MACROS
# define UNKNOWN 0
# define GET 1
# define POST 2
# define DELETE 3

namespace ws {
	class Requests {
		public:
			Requests();
			~Requests();
			int concatenateRequest(std::string buf);
			int requestReceptionState();
			void identifyBodyLengthInHeader(void);
			int checkHeaderEnd() const;
			void findMethod(void);
			int fillHeaderAndBody(void);
			
			// GETTERS
			std::string getRawContent(void) const;
			int getMethodType(void) const;
			int getContentLength(void) const;
			int getHeaderSizeRevieved(void) const;
			int getBodySizeRecieved(void) const;
			int getHeaderSize(void) const;
			std::string getHeader(void) const;
			std::string getBody(void) const;

		private:
			int _state;
			std::string _raw_content;
			int _body_len_recieved;
			int _header_len_recieved;
			int _content_length;
			int _method_type;
			int _header_size;
			std::string _header;
			std::string _body;
	};
}

#endif