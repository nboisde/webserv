#include "Response.hpp"
#include <ctime>

namespace ws
{

std::map<int, std::string>	init_responseMap( void )
{
	std::map<int, std::string> m;

	m[100] = "Continue";
	m[200] = "OK";
	m[300] = "Multiple Choice";
	m[301] = "Moved Permanently";
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[405] = "Not Allowed";
	m[413] = "Request Entity Too Large";
	m[500] = "Internal Server Error";
	return	m;
}

std::map<int, std::string>  Response::_status_code = init_responseMap();

Response::Response( void ) 
{

	_status_line = genStatusLine(OK);
	_status = OK;
}

Response::Response( Response const & src){
	*this = src;
}

Response::~Response( void ){}

Response &  Response::operator=( Response const & rhs)
{
	if ( this != &rhs )
	{ 
		_response = rhs.getResponse();
		_status_line = rhs.getStatusLine();
		_header = rhs.getHeader();
		_body = rhs.getBody();
		_status = rhs.getStatus();
	}
	return *this;
}

//METHODS - //

std::string 	Response::genStatusLine( int status )
{
	int ret_code = OK;
	std::stringstream ret;

	std::map<int, std::string>::iterator it = _status_code.find(status);
	if (it == _status_code.end())
		ret << "HTTP/1.1 " << ret_code << " " << _status_code[ret_code];
	else
		ret << "HTTP/1.1 " << status << " " << _status_code[status];	
	return ret.str();
}

std::string		Response::genDate( void ){
	time_t rawtime;
	struct tm * gmt;
	std::stringstream ret;
	static const char days[][7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char months[][12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	//TIME init and struct loading
	time(&rawtime);
	gmt = gmtime(&rawtime);

	//Header Date String Construction
	ret << "Date: " << days[gmt->tm_wday] << ", ";
	if (gmt->tm_mday < 10)
		ret << "0";
	ret << gmt->tm_mday << " ";
	ret << months[gmt->tm_mon] << " ";
	ret << gmt->tm_year + 1900 << " ";
	if (gmt->tm_hour < 10)
		ret << "0";
	ret << gmt->tm_hour << ":";
	if (gmt->tm_min < 10)
		ret << "0";
	ret << gmt->tm_min << ":";
	if (gmt->tm_sec < 10)
		ret << "0";
	ret << gmt->tm_sec << " GMT";
	
	return ret.str();
}

std::string 	Response::genConnection( void )
{
	std::string co = "Connection: ";
	if (_status == BAD_REQUEST)
		co += "close";
	else
		co += "keep-alive";
	return co;
}

std::string 	Response::genBody( int error )
{
	std::stringstream body;

	body << "<!DOCTYPE html>\n<html>\n<head>\n<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"http:/imgs/fire.ico\"/>\n";
	body << "<title>" << error << " " << _status_code[error] << "</title>\n";
	body << "</head>\n<body>\n";
	body << "<h1>" << error << " " << _status_code[error] << "</h1>\n";
	body << "</body>\n</html>\r\n";

	return body.str();
}

void	Response::setContentType( std::string file_path )
{
	std::string	default_type = "text/html";
	int 		pos = file_path.find_last_of(".");
	
	if (pos < 0)
		_content_type = default_type;
	else
	{
		std::string extension = file_path.substr(pos);
		if (extension == ".gif" || extension == ".ico" || extension == ".png" || extension == ".jpeg")
			_content_type = "image/" + extension.substr(1);
		else if (extension == ".css")
			_content_type = "text/" + extension.substr(1);
		else if (extension == ".pdf")
			_content_type = "application/" + extension.substr(1);
		else
			_content_type = default_type;
	}
}

void	Response::setContentDisposition( std::string file_path )
{
	int			attachement = file_path.find("/download/");
	
	if (attachement < 0)
		_content_disposition = "inline";
	else
		_content_disposition = "attachement";
}

std::string	const &	Response::genHeader( void ){

	//ADD MORE FIELDS IN HEADER (CONTENT LENGHT ETC ETC)
	addContentLength();
	if (_status == BAD_REQUEST)
	{
		_header += CRLF;
		_header += genConnection();
	}
	return _header;
}

const char *	Response::response( int status )
{
	std::stringstream tmp;
	
	if (!_header.empty())
		_header += CRLF;
	_header += genDate();
	if (status != CGI_FLAG)
	{
		_status_line = genStatusLine(status);
		_status = status;
		addContentType();
		addContentDisposition();
	}
	tmp << _status_line << CRLF;
	genHeader();
	tmp << _header << CRLF;
	tmp << CRLF;
	tmp << _body;
	_response = tmp.str();

	const char * str = _response.c_str();

	resetResponse();
	return str;
}

void	Response::addContentLength( void ){
	std::stringstream length;
	
	length << "Content-Length: " << _body.size();
	_header += CRLF;
	_header += length.str();
}


void	Response::addContentType( void )
{
	if (_content_type.empty())
		return ;
	std::stringstream type;
	
	type << "Content-Type: " << _content_type;
	_header += CRLF;
	_header += type.str();
}

void	Response::addContentDisposition( void )
{
	if (_content_disposition.empty())
		return;
	std::stringstream type;
	
	type << "Content-Disposition: " << _content_disposition;
	_header += CRLF;
	_header += type.str();
}

void		Response::treatCGI( std::string cgi_output )
{
	int pos1 = 0;
	int pos2 = 0;

	if ((pos1 = cgi_output.find("Status: ")) != -1)
	{
		pos1 += 8;
		_status_line = "HTTP/1.1 ";	
		_status_line +=  cgi_output.substr(pos1, cgi_output.find(CRLF, pos1) - pos1);
		for(;cgi_output[pos1] != '\n';pos1++);
		pos1++;
	}
	if (pos1 == -1)
		pos1 = 0;
	pos2 = cgi_output.find(BODY_CRLF);
	_header += cgi_output.substr(pos1, pos2);
	if ((pos1 = cgi_output.find(BODY_CRLF)) != -1 && !cgi_output.substr(pos1 + 4).empty())
		_body = cgi_output.substr(pos1 + 4);
}

void		Response::resetResponse( void )
{
	_status_line.clear();
	_header.clear();
	_status = OK;
	_status_line = genStatusLine(_status);
}

void			Response::addToHeader( std::string line ){ _header += line; }

//ACCESSORS - GETTERS//

std::string 	Response::getResponse( void ) const { return _response; }
std::string 	Response::getContentType( void ) const { return _content_type; }
std::string		Response::getStatusLine(void) const { return _status_line; }
std::string		Response::getHeader( void ) const { return _header; }
std::string		Response::getBody( void ) const { return _body; }
int				Response::getStatus( void ) const { return _status; }

//ACCESSORS - SETTERS//
void			Response::setResponse( std::string resp ){ _response = resp; }
void			Response::setStatusLine( std::string status_l ){ _status_line = status_l; }
void			Response::setHeader( std::string header ){ _header = header; }
void			Response::setBody( std::string newbody ){ _body = newbody; }
void			Response::setStatus ( int status ) { _status = status; }

}
