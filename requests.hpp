#ifndef REQUESTS_HPP
# define REQUESTS_HPP

# include <cstring>

// State MACROS
# define REQUEST_FORMAT_ERROR -1
# define RECIEVING_HEADER 0
# define HEADER_RECIVED 1
# define BODY_RECIEVED 2
# define REQUEST_FULL 3

// Methods MACROS
# define UNKNOWN 0
# define GET 1
# define POST 2
# define DELETE 3

namespace ws {
	class Requests {
		public:
			Requests();
			~Resquest();
			int concatenateRequest(std::string buf);
			int requestReceptionState();
			void identifyBodyLengthInHeader(void);
			int checkHeaderEnd() const;
		
		private:
			int _state;
			std::string _raw_content;
			int _body_size_recieved;
			int _header_size_recieved;
			long _content_length;
			int _method_type;
	};
}

#endif