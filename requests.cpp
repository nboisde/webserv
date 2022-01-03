#include <requests.hpp>

/*
REQUEST_FORMAT_ERROR -1
RECIEVING_HEADER 0
HEADER_RECIVED 1
BODY_RECIEVED 2
REQUEST_FULL 3
*/

void ws::Requests::Requests(): _state(RECIEVING_HEADER), _raw_content(NULL), _header_size_recieved(0), _body_size_recieved(0), _content_length(0), _method_type(UNKNOWN){}
void ws::Requests::~Resquest(){}

/*
** this function will concatenate buffer into raw requests and tell to the engine if the request is full: ie,
** if the header and the body is full.
** in This case we will stop using the recv() function and handle with other functions for example in subclass,
** each method separately GET POST DELETE
**
** return values: 
** -1 -> ERROR
** 1 REQUEST_FULL -> Stop recv() -> begin send()
** 0 REQUEST_NOT_FULL -> continue to recv() to catch informations.
*/

int ws::Requests::checkHeaderEnd(void) const
{
	int ret = _raw_content.find("\r\n\r\n");
	if (ret == -1)
		return (0);
	else
		return (1);
}

void ws::Requests::identifyBodyLengthInHeader(void)
{
	std::string l = "Content-Length";
	std::string cl;
	int ret = _raw_content.find(l);
	if (ret == -1)
		return ;
	else
	{
		int i = 0;
		while (_raw_content[ret + l.length() + 2 + i] != '\r')
		{
			cl += str[ret + l.length() + 2 + i];
			i++;
		}
		_content_length = std::stol(cl, NULL, 10);
	}
}

int ws::Requests::concatenateRequest(std::string buf)
{
	_raw_content += buf;

	if (request ReceptionState() == ERROR_REQUEST)
		return (-1);
	else if (requestReceptionState() == REQUEST_FULL)
		return (1);
	else if (requestReceptionState() == RECIEVING_HEADER)
	{
		_header_size_recieved += buf.length();
		if (checkHeaderEnd() == 0)
		{
			identifyBodyLengthInHeader();
			_state = HEADER_RECIEVED;
		}
		return (0);
	}
	else if (requestReceptionState() == HEADER_RECIEVED)
	{
		_body_size_recieved += buf.length();
		if (_body_size_recieved >= _content_length)
		{
			_state = REQUEST_FULL;
			return (1);
		}
	}
}

int ws::Requests::requestReceptionState(void){
	return _state;
}