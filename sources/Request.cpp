#include "Request.hpp"

namespace ws{

Request::Request( void ):
_line(0),
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

// This function must be refined.
int Request::concatenateRequest(std::string buf)
{
	_raw_content += buf;

	// Faire une gestion d'erreur vraiment propre.
	//Seulement une fois le HEADER RECU EN ENTIER !!!!!
	//ICI PROBLEMATIQUE AVEC UNE TAILLE DE BUFFER QUI NE CONTIENT PAS TOUTE LA PREMIERE LIGNE DU HEADER !!!!
	if (_line == 0)
	{
		findMethod();
		if (_method_type == UNKNOWN || !findProtocol(buf))
		{
			_state = REQUEST_FORMAT_ERROR;
			return -1;
		}
	}

	//VARIANLE POURRIE A CHIER.
	_line++;
	if (requestReceptionState() == REQUEST_FORMAT_ERROR)
		return -1;
	if (requestReceptionState() == BODY_RECEIVED)
		return 1;
	if (requestReceptionState() == RECEIVING_HEADER)
	{
		_header_len_received += buf.length();
		if (checkHeaderEnd() == 1)
		{
			// IDENTIFY CONTENT LENGTH AND TRANSFER ENCODING. (if both render -> flag goes to REQUEST_FORMAT_ERROR)
			int ct = identifyBodyLengthInHeader();
			int te = isTransferEncoding();
			findMethod();
			errorHandling();
			if (_method_type == UNKNOWN || (ct == 1 && te == 1))
			{
				_state = REQUEST_FORMAT_ERROR;
				return -1;
			}
			else if (ct == 1)
				_body_reception_encoding = CONTENT_LENGTH;
			else if (te == 1)
				_body_reception_encoding = TRANSFER_ENCODING;
			_state = HEADER_RECEIVED;
		}
	}
	if (requestReceptionState() == HEADER_RECEIVED)
	{
		_body_len_received += buf.length();
		int i = _raw_content.find("\r\n\r\n");
		// ICI -> gerer les differentes methodes d'encodage.
		if (_body_reception_encoding == BODY_RECEPTION_NOT_SPECIFIED)
		{
			_state = BODY_RECEIVED;
			return 1;	
		}
		else if (_body_reception_encoding == CONTENT_LENGTH && _body_len_received + _header_len_received < _content_length + i + 4)
				return 0;
		else if (_body_reception_encoding == TRANSFER_ENCODING)
		{
			int ret = _raw_content.find("0\r\n\r\n");
			if (ret == -1)
				return 0;
			else
			{
				_state = BODY_RECEIVED;
				return 1;
			}
		}
		else
		{
			_state = BODY_RECEIVED;
			return 1;
		}
	}
	return 0;
}


// GERER LA GESTION D'ERREUR PROPRE ICI ! A retourner dans parse HEADER, puis a retourner via fill header and body !
int Request::errorHandling(std::vector<std::string> v)
{
	findMethod();
	//gestion de la presence de l'url requise.
	if (_method_type == UNKNOWN || findProtocol(_header))
	{
		_state = REQUEST_FORMAT_ERROR;
		return ERROR;
	}
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++)
	{
		std::cout << "[" << *it << "]" << std::endl;
	}
	return 1;
}

int		Request::parseHeader(void)
{
	std::vector<std::string> v;
	//_head["url"] = _url;
	//_head["host"] = _host;
    int ret = _header.find("\r\n");
	std::string tmp = _header;
	while (ret != -1)
	{
		v.push_back(tmp.substr(0, ret));
		tmp = tmp.substr(ret + 2, tmp.length() - ret);
		ret = tmp.find("\r\n");
		if (tmp.find("\r\n") == 0)
			break ;
	}
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
		ret = (*it).find(": ");
		_head[(*it).substr(0, ret)] = (*it).substr(ret + 2, (*it).length());
	}
	return 1;
 	//for (std::map<std::string, std::string>::iterator it = _head.begin(); it != _head.end(); it++)
	//	std::cout << (*it).first << "->" << (*it).second << std::endl;
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
	parseHeader();
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
		ChunkedBodyProcessing(body);
/*  		while (_raw_content[i])
		{
			_body += _raw_content[i];
			i++;
		} */
	}
	return 1;
}

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


int Request::requestReceptionState(void)
{
	return _state;
}

std::string Request::getRawContent(void) const
{
	return _raw_content;
}

int Request::getMethodType(void) const
{
	return _method_type;
}

int Request::getContentLength(void) const
{
	return _content_length;
}

int Request::getHeaderSizeRevieved(void) const
{
	return _header_len_received;
}

int Request::getBodySizeReceived(void) const
{
	return _body_len_received;
}

int Request::getHeaderSize(void) const
{
	return _header_size;
}

std::string Request::getHeader(void) const
{
	return _header;
}

std::string Request::getBody(void) const
{
	return _body;
}

int Request::getState( void ) const
{
	return _state;
}

}
