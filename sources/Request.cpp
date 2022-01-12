#include "Request.hpp"

namespace ws{

Request::Request( void ):
_line(0),
_state(RECIEVING_HEADER),
_raw_content(""),
_body_reception_encoding(BODY_RECEPTION_NOT_SPECIFIED),
_body_len_recieved(0),
_header_len_recieved(0),
_content_length(0),
_method_type(UNKNOWN),
_header_size(0),
_header(""),
_body("")
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

int Request::identifyBodyLengthInHeader(void)
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
		return 0;
	else
	{
		int i = 0;
		while (_raw_content[ret + l.length() + 2 + i] != '\r')
		{
			cl += _raw_content[ret + l.length() + 2 + i];
			i++;
		}
		// remplace by strtol
		_content_length = std::stoi(cl, NULL, 10);
		return 1;
	}
}

int Request::isTransferEncoding(void) const
{
	// HERE SEE DIFFERENTS TYPOS OF TRANSFER ENCODING.
	int r = _raw_content.find("Transfer-Encoding: chunked");
	if (r == -1)
		return 0;
	else
		return 1;
}

int		Request::findProtocol(std::string buf)
{
	int ret = buf.find("HTTP/1.1");
	if (ret != -1)
		return 1;
	return 0;
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
	if (_line == 0)
	{
		findMethod();
		if (_method_type == UNKNOWN || !findProtocol(buf))
		{
			_state = REQUEST_FORMAT_ERROR;
			return -1;
		}
	}

	_line++;
	if (requestReceptionState() == REQUEST_FORMAT_ERROR)
		return -1;
	if (requestReceptionState() == BODY_RECIEVED)
		return 1;
	if (requestReceptionState() == RECIEVING_HEADER)
	{
		_header_len_recieved += buf.length();
		if (checkHeaderEnd() == 1)
		{
			// IDENTIFY CONTENT LENGTH AND TRANSFER ENCODING. (if both render -> flag goes to REQUEST_FORMAT_ERROR)
			int ct = identifyBodyLengthInHeader();
			int te = isTransferEncoding();
			findMethod();
			if (_method_type == UNKNOWN || (ct == 1 && te == 1))
			{
				_state = REQUEST_FORMAT_ERROR;
				return -1;
			}
			else if (ct == 1)
				_body_reception_encoding = CONTENT_LENGTH;
			else if (te == 1)
				_body_reception_encoding = TRANSFER_ENCODING;
			_state = HEADER_RECIEVED;
		}
	}
	if (requestReceptionState() == HEADER_RECIEVED)
	{
		_body_len_recieved += buf.length();
		int i = _raw_content.find("\r\n\r\n");
		// ICI -> gerer les differentes methodes d'encodage.
		if (_body_reception_encoding == BODY_RECEPTION_NOT_SPECIFIED)
		{
			_state = BODY_RECIEVED;
			return 1;	
		}
		else if (_body_reception_encoding == CONTENT_LENGTH && _body_len_recieved + _header_len_recieved < _content_length + i + 4)
				return 0;
		else if (_body_reception_encoding == TRANSFER_ENCODING)
		{
			int ret = _raw_content.find("0\r\n\r\n");
			if (ret == -1)
				return 0;
			else
			{
				_state = BODY_RECIEVED;
				return 1;
			}
		}
		else
		{
			_state = BODY_RECIEVED;
			return 1;
		}
	}
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
	std::string body = _raw_content.substr(ret + 4, _raw_content.length() - i);
	//std::cout << body << std::endl;
	if (_body_reception_encoding == BODY_RECEPTION_NOT_SPECIFIED)
		return 1;
	else if (_body_reception_encoding == CONTENT_LENGTH)
	{
		while (_raw_content[i])
		{
			_body += _raw_content[i];
			i++;
		}
	}
	else if (_body_reception_encoding == TRANSFER_ENCODING)
	{
		//ChunkedBodyProcessing(body);
		// ICI, TRAITER LE PARSING DU BODY CHUNKED...
		while (_raw_content[i])
		{
			_body += _raw_content[i];
			i++;
		}
	}
	return 1;
}

int		Request::isHexa(char c)
{
	if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9'
	|| c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f')
		return 1;
	return 0;
}

void		Request::ChunkedBodyProcessing(std::string body)
{
	//(void)body;
	int i = 0;
	int ret = body.find("0\r\n\r\n");
	const char* str = body.c_str();
	int k = 0;
	while (i <= ret)
	{
		std::string proceed_body = "";
		std::string hex = "";
		while (str[i] != '\r')
		{
			hex += str[i];
			i++;
		}
		i+=2;
		std::cout << str[i] << std::endl;
		//std::cout << body[i];
		int j = 0;
		int dec = std::stoi(hex, 0, 16);
		std::cout << hex << ", " << dec << std::endl;
		while (j < dec)
		{
			proceed_body += str[i];
			i++;
			j++;
		}
		int n = 0;
		while (str[i + 4] != 'f' &&str[i + 5] != 'f' &&str[i + 6] != 'f' && str[i + 7] != '4')
		{
			i++;
			n++;
		}
		std::cout << str[i - 1] << std::endl;
		std::cout << str[i] << str[i + 1] << str[i + 2] << str[i + 3] << std::endl;
		std::cout << n << std::endl;
		//i+=2;
		//while (!isHexa(body[i]))
		//	i++;
		std::cout << proceed_body.length() << std::endl;
		_body += proceed_body;
		//ret = _body.find("0\r\n\r\n");
		k++;
		if (k == 3)
			break;
	}
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
