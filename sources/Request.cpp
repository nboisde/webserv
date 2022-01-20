#include "Request.hpp"

namespace ws{

Request::Request( void ):
_line(0),
_cursor(0),
_state(RECEIVING_HEADER),
_raw_content(""),
_body_reception_encoding(BODY_RECEPTION_NOT_SPECIFIED),
_body_len_received(0),
_header_len_received(0),
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
		_body_len_received = rhs._body_len_received;
		_header_len_received = rhs._header_len_received;
		_content_length = rhs._content_length;
		_method_type = rhs._method_type;
		_header_size = rhs._header_size;
		_header = rhs._header;
		_body = rhs._body;
		_head = rhs._head;
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

void Request::findMethod(void)
{
	int g, p, d;
	g = _raw_content.find("GET");
	p = _raw_content.find("POST");
	d = _raw_content.find("DELETE");
	if (g != -1)
	{
		_head["Method"] = "GET";
		_method_type = GET;
	}
	else if (p != -1)
	{
		_head["Method"] = "POST";
		_method_type = POST;
	}
	else if (d != -1)
	{
		_head["Method"] = "UPDATE";
		_method_type = DELETE;
	}
	else
	{
		_head["Method"] = "UNKNOWN";
		_method_type = UNKNOWN;
	}
}

/*
** This function get the value of content_length if it exists.
*/

int Request::identifyBodyLengthInHeader(void)
{
	std::string l = "content-length"; // Change with all compatible casses. ?
	std::string l2 = "Content-Length";
	std::string cl;
	int r1 = _raw_content.find(l);
	int r2 = _raw_content.find(l2);
	int ret = ((r1 == -1 && r2 == -1) || r1 >= 0) ? r1 : r2;
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
	int r = _raw_content.find("Transfer-Encoding:");
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

int Request::bodyReceived(void)
{
	_line = 0;
	_state = BODY_RECEIVED;
	return SUCCESS;
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

// BUG ON THE CONTROL C !!!!!
int Request::concatenateRequest(std::string tmp)
{
	int nl = 0;
	std::string buf;
	if (_line == 0)
	{
		while (tmp[nl] == '\n')
			nl++;
		if (nl == static_cast<int>(tmp.length()))
			return 0;
		else
		{
			while (nl < static_cast<int>(tmp.length()))
			{
				buf += tmp[nl];
				nl++;
			}
		}
	}
	else
		buf = tmp;
	
	_raw_content += buf;

	// ICI: check la premiere ligne ! Check si le protocol est bon.
	if (_line == 0)
	{
		int r = _raw_content.find("\r\n");
		int r2 = _raw_content.find("\n");
		if ((r2 != -1) || r != -1)
		{
			findMethod();
			_line++;
			if (_method_type == UNKNOWN || !findProtocol(_raw_content))
				return errorReturn();
		}
	}

	if (requestReceptionState() == REQUEST_FORMAT_ERROR)
		return ERROR;
	if (requestReceptionState() == BODY_RECEIVED)
		return SUCCESS;
	if (requestReceptionState() == RECEIVING_HEADER)
	{
		_header_len_received += buf.length();
		//EDIT THE VECTOR AND LAUNCH ERROR FUNCTION !
		std::string tmp2 = _raw_content.substr(_cursor, _raw_content.length() - _cursor);
		int i = tmp2.find("\r\n");
		int i2 = tmp2.find("\n");
		std::string crlf = ((i == -1 && i2 == -1) || i != -1) ? "\r\n" : "\n";
		int ret = ((i == -1 && i2 == -1) || i != -1) ? i : i2;
		while (ret != -1)
		{
			_vheader.push_back(tmp2.substr(0, ret));
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
/* 		for (std::vector<std::string>::iterator it = _vheader.begin(); it != _vheader.end(); it++)
			std::cout << (*it) << std::endl; */
		if (errorHandling(_vheader, 2) == ERROR)
			return errorReturn();
		if (checkHeaderEnd() == 1)
		{
			_cursor = 0;
			_vheader.clear();
			int ct = identifyBodyLengthInHeader();
			int te = isTransferEncoding();
			//if (_method_type == UNKNOWN)
			//	findMethod();
			if (_method_type == UNKNOWN)// || (ct == 1 && te == 1))
				return errorReturn();
			/* else  */if (ct == 1)
				_body_reception_encoding = CONTENT_LENGTH;
			else if (te == 1)
				_body_reception_encoding = TRANSFER_ENCODING;
			_state = HEADER_RECEIVED;
		}
	}
	//BODY CATCH
	if (requestReceptionState() == HEADER_RECEIVED)
	{
		_body_len_received += buf.length();
		int r = _raw_content.find("\r\n\r\n");
		int r2 = _raw_content.find("\n\n");
		int i = (r == -1) ? r2 : r;
		// Attention ici a gerer si la content length ne match jamais la reception...
		if (_body_reception_encoding == BODY_RECEPTION_NOT_SPECIFIED)
			return bodyReceived();
		else if (_body_reception_encoding == CONTENT_LENGTH && _body_len_received + _header_len_received < _content_length + i + 4)
				return 0;
		else if (_body_reception_encoding == TRANSFER_ENCODING)
		{
			// ICI OBSERVER SI LE BODY FINIT BIEN PAR \r\n\r\n ou \n\n -> a mon avis ne doit pas etre traite.
			int ret = _raw_content.find("0\r\n\r\n");
			if (ret == -1)
				return 0;
			else
				return bodyReceived();
		}
		else
			return bodyReceived();
	}
	return 0;
}

int Request::errorReturn(void)
{
	_state = REQUEST_FORMAT_ERROR;
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
	//gestion d'une taille de header trop grosse.
	int cl = 0;
	int te = 0;
	int host = 0;
	if (_method_type == UNKNOWN || !findProtocol(_raw_content))
		return errorReturn();
	for (std::vector<std::string>::iterator it = v.begin() + 1; it != v.end(); it++)
	{
		int len = static_cast<int>((*it).length());
		int ret = (*it).find(":");
		if (host == 0)
			if (static_cast<int>((*it).find("Host")) != -1 && ret != -1)
				host = 1;
		//NO double dot.
		if (ret == -1)
		{
			if (static_cast<int>((*it).find(" ")) != -1)
				return errorReturn();
			else
				continue ;
		}
		//	return errorReturn();
		//EMPTY FIELD.
		if (ret == 0 || (*it)[ret - 1] == ' ')
			return errorReturn();
		//DEAL WITH SPACES.
		for (int i = 0; i < ret; i++)
			if ((*it)[i] == ' ')
				return errorReturn();
		//NULL CHARACTER IN VALUE.
		for (int i = ret; i < len; i++)
			if ((*it)[i] == '\0')
				return errorReturn();
		if (static_cast<int>((*it).find("Content-Length")) != -1)
		{
			// ONLY NUMBER IN THE RIGHT FORMAT !
			int i  = ret + 1;
			while (i < len)
			{
				if ((*it)[i] != ' ')
				{
					if (!std::isdigit((*it)[i]))
						return errorReturn();
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
					return errorReturn();
				i++;
			}
			cl++;
		}
		if (static_cast<int>((*it).find("Transfer-Encoding")) != -1)
		{
			int chunk = (*it).find("chunked");
			//Because we only deal with chunked transfer encoding method
			if (chunk == -1)
				return errorReturn();
			for (int i = ret + 1; i < chunk; i++)
			{
				if ((*it)[i] && (*it)[i] != ' ')
					return errorReturn();
			}
			for (int i = chunk + 7; i != len; i++)
			{
				if ((*it)[i] != ' ')
					return errorReturn();
			}
			te++;
		}
	}
	// NOT A COMBINAISON OF TE.CL or TE.TE or CL.CL
	if ((te == 1 && cl == 1) || te > 1 || cl > 1)
		return errorReturn();
	if (i == 1 && host == 0)
		return errorReturn();
	return SUCCESS;
}

/*
** This function parses the header and put data into _head map (also render error if the header format isn't respected).
*/

int		Request::parseHeader(void)
{
	std::vector<std::string> v;
    int i = _header.find("\r\n");
	int i2 = _header.find("\n");
	std::string crlf = ((i == -1 && i2 == -1) || i != -1) ? "\r\n" : "\n";
	int ret = ((i == -1 && i2 == -1) || i != -1) ? i : i2;
	std::string tmp = _header;
	while (ret != -1)
	{
		v.push_back(tmp.substr(0, ret));
		tmp = tmp.substr(ret + crlf.length(), tmp.length() - ret);
		ret = tmp.find(crlf);
		if (tmp.find(crlf) == 0)
			break ;
	}
	int err = errorHandling(v, 1);
	if (err == ERROR)
		return errorReturn();
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
		// Doesn't put in header data empty lines. // Don't know if we must consider...
		if (ret == -1)
			continue ;
		_head[(*it).substr(0, ret)] = (*it).substr(ret + 2, (*it).length());
	}
	//for (std::map<std::string, std::string>::iterator it = _head.begin(); it != _head.end(); it++)
	//	std::cout << (*it).first << "->" << (*it).second << std::endl;
	return SUCCESS;
}

/*
** This function wait for all the recv to be processed and split raw content into header and body.
** Return ERROR i.e -1 if problem in header format.
*/

int Request::fillHeaderAndBody(void){
	int r = _raw_content.find("\r\n\r\n");
	int r2 = _raw_content.find("\n\n");
	int ret = (r != -1) ? r : r2;
	std::string crlf = (r != -1) ? "\r\n\r\n": "\n\n";
	int i = 0;
	if (ret == -1)
		return errorReturn();
	while (i < ret + static_cast<int>(crlf.length()))
	{
		_header += _raw_content[i];
		i++;
	}
	std::string body = _raw_content.substr(ret + crlf.length(), _raw_content.length() - i);
	int err = parseHeader();
	if (err == ERROR)
		return errorReturn();
	if (_body_reception_encoding == BODY_RECEPTION_NOT_SPECIFIED)
		return SUCCESS;
	else if (_body_reception_encoding == CONTENT_LENGTH)
	{
		while (_raw_content[i])
		{
			_body += _raw_content[i];
			i++;
		}
	}
	else if (_body_reception_encoding == TRANSFER_ENCODING)
		ChunkedBodyProcessing(body);
	return SUCCESS;
}

/*
** This function implement information catching of chunked requests and concatenate chunks into the body.
*/

void		Request::ChunkedBodyProcessing(std::string body)
{
	int i = 0;
	int ret = body.find("0\r\n\r\n");
	const char* str = body.c_str();
	while (i < ret)
	{
		std::string proceed_body = "";
		std::string hex = "";
		while (str[i] != '\r' && str[i + 1] != '\n')
		{
			hex += str[i];
			i++;
		}
		i+=2;
		int j = 0;
		int dec = std::strtol(hex.c_str(), 0, 16);
		while (j < dec)
		{
			proceed_body += str[i];
			i++;
			j++;
		}
		i+=2;
		_body += proceed_body;
	}
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
int										Request::getState(void) const { return _state; }
std::map<std::string, std::string>		Request::getHead(void) const { return _head; }

}
