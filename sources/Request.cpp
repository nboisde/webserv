#include "Request.hpp"

namespace ws{

Request::Request( void ): _state(RECIEVING_HEADER), _raw_content(""),  _body_len_recieved(0), _header_len_recieved(0), _content_length(0), _method_type(UNKNOWN), _header_size(0), _header(""), _body("")
{

}

Request::Request( Request const & src)
{
	*this = src;
}

Request::~Request()
{

}

Request &	Request::operator=( Request const & rhs )
{
	if ( this != &rhs )
	{
		_state = rhs._state;
		_raw_content = rhs._raw_content;
		_body_len_recieved = rhs._body_len_recieved;
		_header_len_recieved = rhs._header_len_recieved;
		_content_length = rhs._content_length;
		_method_type = rhs._method_type;
		_header_size = rhs._header_size;
		_header = rhs._header;
		_body = rhs._body;
	}
	return *this;
}

int Request::checkHeaderEnd(void) const
{
	int ret = _raw_content.find("\r\n\r\n");
	//std::cout << "find end header" << ret << std::endl;
	if (ret == -1)
		return (0);
	else
		return (1);
}

void Request::findMethod(void)
{
	int g, p, d;
	g = _raw_content.find("GET");
	p = _raw_content.find("POST");
	d = _raw_content.find("DELETE");
	if (g != -1)
		_method_type = GET;
	else if (p != -1)
		_method_type = POST;
	else if (d != -1)
		_method_type = DELETE;
	else
		_method_type = UNKNOWN;
}

void Request::identifyBodyLengthInHeader(void)
{
	std::string l = "content-length"; // TO CHANGE TO BE COMPATIBLE WITH CONTENT-LENGTH ALL CASES.. ex: postman computer.
	std::string l2 = "Content-Length";
	std::string cl;
	int r1 = _raw_content.find(l);
	int r2 = _raw_content.find(l2);
	int ret;
	if ((r1 == -1 && r2 == -1) || r1 >= 0)
		ret = r1;
	else
		ret = r2;
	if (ret == -1)
		return ;
	else
	{
		int i = 0;
		while (_raw_content[ret + l.length() + 2 + i] != '\r')
		{
			cl += _raw_content[ret + l.length() + 2 + i];
			i++;
		}
		_content_length = std::stoi(cl, NULL, 10);
	}
}

/*
** this function will concatenate buffer into raw Request and tell to the engine if the request is full: ie,
** if the header and the body is full.
** in This case we will stop using the recv() function and handle with other functions for example in subclass,
** each method separately GET POST DELETE
**
** return values: 
** -1 -> ERROR
** 1 REQUEST_FULL -> Stop recv() -> begin send()
** 0 REQUEST_NOT_FULL -> continue to recv() to catch informations.
*/

int Request::concatenateRequest(std::string buf)
{
	_raw_content += buf;
	//std::cout << _raw_content << std::endl << std::endl;

	if (requestReceptionState() == REQUEST_FORMAT_ERROR)
		return -1;
	else if (requestReceptionState() == BODY_RECIEVED)
		return 1;
	else if (requestReceptionState() == RECIEVING_HEADER)
	{
		_header_len_recieved += buf.length();
		if (checkHeaderEnd() == 1)
		{
			identifyBodyLengthInHeader();
			findMethod();
			if (_method_type == UNKNOWN)
			{
				_state = REQUEST_FORMAT_ERROR;
				return -1;
			}
			_state = HEADER_RECIEVED;
		}
		return 0;
	}
	else if (requestReceptionState() == HEADER_RECIEVED)
	{
		_body_len_recieved += buf.length();
		int i = _raw_content.find("\r\n\r\n");
		if (_body_len_recieved + _header_len_recieved < _content_length + i + 4)
			return 0;
		else
		{
			//fillHeaderAndBody();
			_state = BODY_RECIEVED;
			return 1;
		}
	}
	else
		return 0;
}

int Request::fillHeaderAndBody(void){
	int ret = _raw_content.find("\r\n\r\n");
	int i = 0;

	if (ret == -1)
		return -1;
	while (i < ret + 4)
	{
		_header += _raw_content[i];
		i++;
	}
	if (_content_length == 0)
		return 0;
	while (_raw_content[i])
	{
		_body += _raw_content[i];
		i++;
	}
	return 1;
}


int Request::requestReceptionState(void){
	return _state;
}

std::string Request::getRawContent(void) const{
	return _raw_content;
}

int Request::getMethodType(void) const{
	return _method_type;
}

int Request::getContentLength(void) const{
	return _content_length;
}
int Request::getHeaderSizeRevieved(void) const{
	return _header_len_recieved;
}
int Request::getBodySizeRecieved(void) const{
	return _body_len_recieved;
}

int Request::getHeaderSize(void) const{
	return _header_size;
}
std::string Request::getHeader(void) const{
	return _header;
}
std::string Request::getBody(void) const{
	return _body;
}

int Request::getState( void ) const
{
	return _state;
}

}