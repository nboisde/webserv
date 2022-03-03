#include "Request.hpp"

namespace ws{

Request::Request( void ):
_line(0),
_cursor(0),
_state(RECEIVING_HEADER),
_connection(KEEP_ALIVE),
_raw_content(""),
_body_reception_encoding(BODY_RECEPTION_NOT_SPECIFIED),
_body_len_received(0),
_header_len_received(0),
_content_length(0),
_method_type(UNKNOWN),
_header_size(0),
_header(""),
_body(""),
_status(OK),
_multipart(0),
_boundary(""),
_continue(0),
_upload_authorized(1)
{

}

Request::Request( int up_auth ):
_line(0),
_cursor(0),
_state(RECEIVING_HEADER),
_connection(KEEP_ALIVE),
_raw_content(""),
_body_reception_encoding(BODY_RECEPTION_NOT_SPECIFIED),
_body_len_received(0),
_header_len_received(0),
_content_length(0),
_method_type(UNKNOWN),
_header_size(0),
_header(""),
_body(""),
_status(OK),
_multipart(0),
_boundary(""),
_continue(0),
_upload_authorized(up_auth)
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
		_body_len_received = rhs._body_len_received;
		_header_len_received = rhs._header_len_received;
		_content_length = rhs._content_length;
		_method_type = rhs._method_type;
		_header_size = rhs._header_size;
		_header = rhs._header;
		_body = rhs._body;
		_head = rhs._head;
		_status = rhs._status;
		_multipart = rhs._multipart;
		_boundary = rhs._boundary;
		_continue = rhs._continue;
		_upload_authorized = rhs._upload_authorized;
	}
	return *this;
}

int Request::checkHeaderEnd(void) const
{
	int ret = _raw_content.find("\r\n\r\n");
	int ret2 = _raw_content.find("\n\n");
	if (ret == -1 && ret2 == -1)
		return (0);
	else
		return (1);
}

int Request::analyseURL(void)
{
	size_t r = _raw_content.find("\r\n");
	size_t r2 = _raw_content.find("\n");
	size_t ret = (r == static_cast<size_t>(-1) && r2 != static_cast<size_t>(-1)) ? r2 : r;
	if (ret == static_cast<size_t>(-1))
		return 0;
	std::string fl = _raw_content.substr(0, ret);
	int i = 0;
	while (fl[i] && fl[i] != ' ')
		i++;
	while (fl[i] == ' ')
		i++;
	if (fl[i] != '/')
	{
		return 0;
	}
	while (fl[i] && fl[i] != ' ')
		i++;
	if (fl[i] == '\0')
		return 0;
	return 1;
}

void Request::findMethod(void)
{
	int g, p, d;
	g = _raw_content.find("GET");
	p = _raw_content.find("POST");
	d = _raw_content.find("DELETE");
	if (g != -1)
	{
		_head["method"] = "GET";
		_method_type = GET;
	}
	else if (p != -1)
	{
		_head["method"] = "POST";
		_method_type = POST;
	}
	else if (d != -1)
	{
		_head["method"] = "DELETE";
		_method_type = DELETE;
	}
	else
	{
		_head["method"] = "UNKNOWN";
		_method_type = UNKNOWN;
	}
}

/*
** This function get the value of content_length if it exists.
*/

int Request::identifyBodyLengthInHeader(void)
{
	std::string l = "content-length";
	std::string cl;
	int ret = strToLower(_raw_content).find(l);
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
		_content_length = std::strtol(cl.c_str(), NULL, 10);
		return 1;
	}
}

int Request::isTransferEncoding(void) const
{
	// HERE SEE DIFFERENTS TYPOS OF TRANSFER ENCODING.
	int r = strToLower(_raw_content).find("transfer-encoding:");
	int r2 = _raw_content.find("chunked");
	if (r == -1 || r2 == -1)
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

void	Request::manageConnection( std::string str )
{
	int ret = strToLower(str).find("connection:");
	if (ret == -1)
		return ;
	else
	{
		int r2 = str.find("close");
		if (r2 != -1)
			_connection = CLOSE;
		return ;
	}
}


int Request::bodyReceived(void)
{
	_line = 0;
	_state = BODY_RECEIVED;
	if (_continue == 1)
		_continue = 0;
	return SUCCESS;
}

int	Request::multipartForm( void )
{
	int r = strToLower(_header).find("content-type");
	int r2 = strToLower(_header).find("multipart");
	if (r == -1 || r2 == -1)
		return 0;
	else
	{
		_multipart = 1;
		std::string tmp = _header.substr(r);
		int bd = strToLower(tmp).find("boundary=");
		std::string tmp2 = tmp.substr(bd + 9, tmp.find("\r\n") - (bd + 9));
		_boundary = tmp2;
		return 1;
	}
}

int	Request::multipartFormRaw( void )
{
	int r = strToLower(_raw_content).find("content-type");
	int r2 = strToLower(_raw_content).find("multipart");
	if (r == -1 || r2 == -1)
		return 0;
	else
	{
		_multipart = 1;
		std::string tmp = _raw_content.substr(r);
		int bd = strToLower(tmp).find("boundary=");
		std::string tmp2 = tmp.substr(bd + 9, tmp.find("\r\n") - (bd + 9));
		_boundary = tmp2;
		return 1;
	}
}

int Request::findContinue( void )
{
	int r = strToLower(_raw_content).find("except");
	int r2 = strToLower(_raw_content).find("100-continue");
	if (r == -1 || r2 == -1)
		return 0;
	else
	{
		_continue = 1;
		return 1;
	}
}

int Request::findMultiEnd( void )
{
	std::string limit = "--" + _boundary + "--\r\n";
	std::string end = _raw_content.substr(_raw_content.length() - limit.length());
	if (limit == end)
		return 1;
	return 0;
}

int Request::findBodyEnd( void )
{
	if (_multipart == 1)
	{
		int ml = findMultiEnd();
		size_t j = _raw_content.find("\r\n\r\n");
		if (!ml || j == static_cast<size_t>(-1) || static_cast<size_t>(_content_length) > _raw_content.substr(j + 4).length())
			return 0;
	}
	else if (_continue == 1)
	{
		size_t i = _raw_content.find("0\r\n\r\n");
		if (i == static_cast<size_t>(-1))
			return 0;
	}
	return bodyReceived();
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

int Request::concatenateRequest(std::string tmp)
{
	int nl = 0;
	std::string buf;
	if (_line == 0)
	{
		while (tmp[nl] == '\n')
			nl++;
		if (static_cast<size_t>(nl) == (tmp.length()))
			return 0;
		else
		{
			while (static_cast<size_t>(nl) < (tmp.length()))
			{
				buf += tmp[nl];
				nl++;
			}
		}
	}
	else
		buf = tmp;
	
	_raw_content += buf;

	if (_line == 0)
	{
		int r = _raw_content.find("\r\n");
		int r2 = _raw_content.find("\n");
		if ((r2 != -1) || r != -1)
		{
			findMethod();
			_line++;
			if (!analyseURL())
				return errorReturn(0);
			if (_method_type == UNKNOWN)
				return errorReturn(1);
			if (!findProtocol(_raw_content))
				return errorReturn(0);
		}
	}

	if (requestReceptionState() == REQUEST_FORMAT_ERROR)
		return ERROR;
	if (requestReceptionState() == BODY_RECEIVED)
		return SUCCESS;
	if (requestReceptionState() == RECEIVING_HEADER)
	{
		_header_len_received += buf.length();
		std::string tmp2 = _raw_content.substr(_cursor, _raw_content.length() - _cursor);
		int i = tmp2.find("\r\n");
		int i2 = tmp2.find("\n");
		std::string crlf = ((i == -1 && i2 == -1) || i != -1) ? "\r\n" : "\n";
		int ret = ((i == -1 && i2 == -1) || i != -1) ? i : i2;
		while (ret != -1)
		{
			_vheader.push_back(tmp2.substr(0, ret));
			manageConnection(tmp2.substr(0, ret));
			if (ret > 0)
				_cursor += ret + crlf.length();
			tmp2 = tmp2.substr(ret + crlf.length(), tmp2.length() - ret);
			ret = tmp2.find(crlf);
			if (tmp2.find(crlf) == 0)
			{
				_cursor -= crlf.length();
				break ;
			}
		}
		if (errorHandling(_vheader, 2) == ERROR)
			return ERROR;
		if (checkHeaderEnd() == 1)
		{
			_cursor = 0;
			_vheader.clear();
			int ct = identifyBodyLengthInHeader();
			int te = isTransferEncoding();
			if (ct == 1)
				_body_reception_encoding = CONTENT_LENGTH;
			else if (te == 1)
				_body_reception_encoding = TRANSFER_ENCODING;
			_state = HEADER_RECEIVED;
			multipartFormRaw();
			findContinue();
		}
	}
	//BODY CATCH
	if (requestReceptionState() == HEADER_RECEIVED)
	{
		_body_len_received += buf.length();
		size_t r = _raw_content.find("\r\n\r\n");
		size_t r2 = _raw_content.find("\n\n");
		size_t i = (r == static_cast<size_t>(-1)) ? r2 : r;
		if (_multipart == 1 || _continue == 1)
			return findBodyEnd();
		else if (_body_reception_encoding == BODY_RECEPTION_NOT_SPECIFIED)
			return bodyReceived();
		else if ((_body_reception_encoding == CONTENT_LENGTH) && (_raw_content.length()) < static_cast<size_t>(_content_length + i + 4))//_body_len_received + _header_len_received < _content_length + i + 4)
			return 0;
		else if (_body_reception_encoding == TRANSFER_ENCODING)
		{
			size_t ret = _raw_content.find("0\r\n\r\n");
			if (ret == static_cast<size_t>(-1))
				return 0;
			else
				return bodyReceived();
		}
		else
			return bodyReceived();
	}
	return 0;
}

int Request::errorReturn( int opt)
{
	_state = REQUEST_FORMAT_ERROR;
	if (opt == 1)
		_status = NOT_ALLOWED;
	if (opt == 2)
		_status = REQUEST_ENTITY_TOO_LARGE;
	else
		_status = BAD_REQUEST;
	return ERROR;
}

// Prevent HACKING of the server: Article reference: https://0xn3va.gitbook.io/cheat-sheets/web-application/http-request-smuggling
// TODO: Gestion des requetes multiples de content-length et transfer-encoding.

/*
** This function handle Errors in the format -> the result will be used in parse header and after that in the return of fillHeaderAndBody.
*/

int Request::errorHandling(std::vector<std::string> v, int i)
{
	//gestion de la presence de l'url requise.
	int cl = 0;
	int te = 0;
	int host = 0;
	if (!analyseURL())
		return errorReturn(0);
	if (_method_type == UNKNOWN)
		return errorReturn(1);
	if (!findProtocol(_raw_content))
		return errorReturn(0);
	for (std::vector<std::string>::iterator it = v.begin() + 1; it != v.end(); it++)
	{
		size_t len = ((*it).length());
		size_t ret = (*it).find(":");
		if (host == 0)
			if ((strToLower((*it)).find("host")) != static_cast<size_t>(-1) && ret != static_cast<size_t>(-1))
				host = 1;
		//NO double dot.
		if (ret == static_cast<size_t>(-1))
		{
			if (((*it).find(" ")) != static_cast<size_t>(-1))
				return errorReturn(0);
			else
				continue ;
		}
		//	return errorReturn();
		//EMPTY FIELD.
		if (ret == static_cast<size_t>(0) || (*it)[ret - 1] == ' ')
			return errorReturn(0);
		//DEAL WITH SPACES.
		for (size_t i = 0; i < ret; i++)
			if ((*it)[i] == ' ')
				return errorReturn(0);
		//NULL CHARACTER IN VALUE.
		for (size_t i = ret; i < len; i++)
			if ((*it)[i] == '\0')
				return errorReturn(0);
		if ((strToLower((*it)).find("content-length")) != static_cast<size_t>(-1))
		{
			// ONLY NUMBER IN THE RIGHT FORMAT !
			size_t i  = ret + 1;
			while (i < len)
			{
				if ((*it)[i] != ' ')
				{
					if (!std::isdigit((*it)[i]))
						return errorReturn(0);
					else if (std::isdigit((*it)[i]))
						break ;
				}
				i++;
			}
			while (std::isdigit((*it)[i]))
				i++;
			while (i < len)
			{
				if ((*it)[i] != ' ')
					return errorReturn(0);
				i++;
			}
			cl++;
		}
		if ((strToLower((*it)).find("transfer-encoding")) != static_cast<size_t>(-1))
		{
			size_t chunk = (*it).find("chunked");
			//Because we only deal with chunked transfer encoding method
			if (chunk == static_cast<size_t>(-1))
				return errorReturn(0);
			for (size_t i = ret + 1; i < chunk; i++)
			{
				if ((*it)[i] && (*it)[i] != ' ')
					return errorReturn(0);
			}
			for (size_t i = chunk + 7; i != len; i++)
			{
				if ((*it)[i] != ' ')
					return errorReturn(0);
			}
			te++;
		}
	}
	// NOT A COMBINAISON OF TE.CL or TE.TE or CL.CL
	if ((te == 1 && cl == 1) || te > 1 || cl > 1)
		return errorReturn(0);
	if (i == 1 && host == 0)
		return errorReturn(0);
	return SUCCESS;
}

/*
** This function parses the header and put data into _head map (also render error if the header format isn't respected).
*/

int		Request::parseHeader(void)
{
	std::vector<std::string> v;
    size_t i = _header.find("\r\n");
	size_t i2 = _header.find("\n");
	std::string crlf = ((i == static_cast<size_t>(-1) && i2 == static_cast<size_t>(-1)) || i != static_cast<size_t>(-1)) ? "\r\n" : "\n";
	size_t ret = ((i == static_cast<size_t>(-1) && i2 == static_cast<size_t>(-1)) || i != static_cast<size_t>(-1)) ? i : i2;
	std::string tmp = _header;
	while (ret != static_cast<size_t>(-1))
	{
		v.push_back(tmp.substr(0, ret));
		tmp = tmp.substr(ret + crlf.length(), tmp.length() - ret);
		ret = tmp.find(crlf);
		if (tmp.find(crlf) == 0)
			break ;
	}
	int err = errorHandling(v, 1);
	if (err == ERROR)
		return ERROR;
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++)
	{
		if (it == v.begin())
		{
			std::string url = "";
			std::string protocol = "";
			int i = 0;
			for (std::string::iterator si = (*it).begin(); si != (*it).end(); si++)
			{
				if (*si && *si == ' ')
				{
					i++;
					while (*si && *si == ' ')
						si++;
				}
				if (i == 1)
					url += *si;
				if (i == 2)
					protocol += *si;
			}
			_head["protocol"] = protocol;
			_head["url"] = url;
			continue ;
		}
		ret = (*it).find(":");
		if (ret == static_cast<size_t>(-1))
			continue ;
		_head[strToLower((*it).substr(0, ret))] = (*it).substr(ret + 2, (*it).length());
	}
	return SUCCESS;
}

/*
** This function wait for all the recv to be processed and split raw content into header and body.
** Return ERROR i.e -1 if problem in header format.
*/

int Request::fillHeaderAndBody(void){
	size_t r = _raw_content.find("\r\n\r\n");
	size_t r2 = _raw_content.find("\n\n");
	size_t ret = (r != static_cast<size_t>(-1)) ? r : r2;
	std::string crlf = (r != static_cast<size_t>(-1)) ? "\r\n\r\n": "\n\n";
	size_t i = 0;
	if (ret == static_cast<size_t>(-1))
		return errorReturn(0);
	while (i < ret + (crlf.length()))
	{
		_header += _raw_content[i];
		i++;
	}
	std::string body = _raw_content.substr(ret + crlf.length(), _raw_content.length() - ret);
	int err = parseHeader();
	if (err == ERROR)
		return errorReturn(0);
	if (_body_reception_encoding == BODY_RECEPTION_NOT_SPECIFIED)
		return SUCCESS;
	else if (_body_reception_encoding == CONTENT_LENGTH)
		_body += _raw_content.substr(i, _content_length);
	else if (_body_reception_encoding == TRANSFER_ENCODING)
		ChunkedBodyProcessing(body);
	multipartForm();
	return SUCCESS;
}

/*
** This function implement information catching of chunked requests and concatenate chunks into the body.
*/

void		Request::ChunkedBodyProcessing(std::string body)
{
	size_t i = 0;
	size_t ret = body.find("0\r\n\r\n");
	int k = 0;
	if (ret == static_cast<size_t>(-1))
	{
		_body = "";
		return ;
	}
	while (body[i] != '0' && i < ret)//i < ret)
	{
		std::string proceed_body = "";
		std::string hex = "";
		while (body[i] != '\r' && body[i + 1] && body[i + 1] != '\n')
		{
			hex += body[i];
			i++;
		}
		i+=2;
		int j = 0;
		int dec = std::strtol(hex.c_str(), 0, 16);
		if (body[i] == '0')
			break ;
		if (dec == 0)
			break ;
		while (j < dec && i < ret)
		{
			proceed_body += body[i];
			i++;
			j++;
		}
		i+=2;
		_body += proceed_body;
		k++;
	}
}

void	Request::resetValues(void){
	//if (_continue == 0)
	//{
	_line = 0;
	_cursor = 0;
	_state = RECEIVING_HEADER;
	_connection = KEEP_ALIVE;
	_raw_content.clear();
	_raw_content = "";
	_body_reception_encoding = BODY_RECEPTION_NOT_SPECIFIED;
	_body_len_received = 0;
	_header_len_received = 0;
	_content_length = 0;
	_method_type = UNKNOWN;
	_header_size = 0;
	_header.clear();
	_header = "";
	_body.clear();
	_body = "";
	_status = OK;
	_head.clear();
	_multipart = 0;
	_boundary.clear();
	_boundary = "";
	_continue = 0;
	_upload_authorized = 1;
	//}
}

void	Request::cleanRawContent(void)
{
	_raw_content.clear();
	_raw_content = "";
}

int										Request::requestReceptionState(void) { return _state; }
std::string								Request::getRawContent(void) const { return _raw_content; }
int										Request::getMethodType(void) const { return _method_type; }
int										Request::getContentLength(void) const { return _content_length; }
int										Request::getHeaderSizeRevieved(void) const { return _header_len_received; }
int										Request::getBodySizeReceived(void) const { return _body_len_received; }
int										Request::getHeaderSize(void) const { return _header_size; }
std::string								Request::getHeader(void) const { return _header; }
std::string								Request::getBody(void) const { return _body; }
std::string	&							Request::getBody(void) { return _body;}
void									Request::setBody(std::string new_body) {_body = new_body;}
int										Request::getState(void) const { return _state; }
std::map<std::string, std::string>		Request::getHead( void ) const { return _head; }
std::map<std::string, std::string> &	Request::getHead( void ) { return _head; }
int										Request::getConnection( void ) const { return _connection; }
int										Request::getStatus( void ) const { return _status; }
int										Request::getMultipart( void ) const { return _multipart; }
std::string								Request::getBoundary( void ) const { return _boundary; }
int										Request::getContinue( void ) const { return _continue; }
int										Request::getUploadAuthorized( void ) const { return _upload_authorized; }


void									Request::setContinue( int cont ){ _continue = cont; }
void									Request::setUploadAuthorized( int up_auth ) { _upload_authorized = up_auth; }
void									Request::setHeadKey( std::string key, std::string value ) { _head[key] = value; }

}
