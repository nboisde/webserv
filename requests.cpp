#include "requests.hpp"

ws::Requests::Requests(): _state(RECIEVING_HEADER), _raw_content(""),  _body_len_recieved(0), _header_len_recieved(0), _content_length(0), _method_type(UNKNOWN), _header_size(0), _header(""), _body(""){}
ws::Requests::~Requests(){}

int ws::Requests::checkHeaderEnd(void) const
{
	int ret = _raw_content.find("\r\n\r\n");
	std::cout << "find end header" << ret << std::endl;
	if (ret == -1)
		return (0);
	else
		return (1);
}

void ws::Requests::findMethod(void)
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

void ws::Requests::identifyBodyLengthInHeader(void)
{
	std::string l = "content-length"; // TO CHANGE TO BE COMPATIBLE WITH CONTENT-LENGTH ALL CASES.. ex: postman computer.
	std::string cl;
	int ret = _raw_content.find(l);
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

int ws::Requests::concatenateRequest(std::string buf)
{
	_raw_content += buf;
	std::cout << _raw_content << std::endl << std::endl;

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
			_state = BODY_RECIEVED;
			return 1;
		}
	}
	else
		return 0;
}

int ws::Requests::fillHeaderAndBody(void){
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


int ws::Requests::requestReceptionState(void){
	return _state;
}

std::string ws::Requests::getRawContent(void) const{
	return _raw_content;
}

int ws::Requests::getMethodType(void) const{
	return _method_type;
}

int ws::Requests::getContentLength(void) const{
	return _content_length;
}
int ws::Requests::getHeaderSizeRevieved(void) const{
	return _header_len_recieved;
}
int ws::Requests::getBodySizeRecieved(void) const{
	return _body_len_recieved;
}

int ws::Requests::getHeaderSize(void) const{
	return _header_size;
}
std::string ws::Requests::getHeader(void) const{
	return _header;
}
std::string ws::Requests::getBody(void) const{
	return _body;
}