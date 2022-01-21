#include "Response.hpp"
#include <ctime>

namespace ws
{

std::map<int, std::string>	init_responseMap( void ){
	std::map<int, std::string>	m;

	m[100] = "Continue";
	m[200] =  "OK";
	m[300] =  "Multiple Choice";
	m[301] =  "Moved Permanently";
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[500] =  "Internal Server Error";
	return m;
}

std::map<int, std::string>  Response::_status_code = init_responseMap();

Response::Response( void ) 
{
	_status_line = genStatusLine();
	_header = genHeader();
};

Response::Response( Response const & src){
	*this = src;
}

Response::~Response( void ){
};

Response &  Response::operator=( Response const & rhs)
{
	if ( this != &rhs )
	{ 
		_response = rhs.getResponse();
		_status_line = rhs.getStatusLine();
		_header = rhs.getHeader();
		_body = rhs.getBody();
	}
	return *this;
}

//METHODS - //

std::string 	Response::genStatusLine( void ){
	int ret_code = 200;
	std::stringstream ret;

	ret << "HTTP/1.1 " << ret_code << " " << _status_code[ret_code];	
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

std::string	const &	Response::genHeader( void ){

	_header += genDate();
	//ADD MORE FIELDS IN HEADER (CONTENT LENGHT ETC ETC)
	
	return _header;
}

const char *      Response::response( void ){
	std::stringstream tmp;
	
	tmp << _status_line << CRLF;
	tmp << _header << CRLF;
	tmp << CRLF;
	tmp << _body;

	_response = tmp.str();

	const char * str = _response.c_str();
	return str;
}

size_t      Response::response_size( void ){
	return  _response.size();
}

void		Response::treatCGI( std::string cgi_output )
{
	int pos;

	if ((pos = cgi_output.find("Status: ")) != -1)
	{
		pos += 8;
		_status_line = "HTTP/1.1 ";	
		_status_line +=  cgi_output.substr(pos, cgi_output.find(CRLF, pos) - pos);
		_body = cgi_output.substr(0, pos - 8);
		_body += cgi_output.substr(cgi_output.find(CRLF, pos) + 2);
	}
	else if ((pos = cgi_output.find(BODY_CRLF)) != -1)
	{
		_body = cgi_output. substr(pos + 4);
		_header += CRLF;
		_header += cgi_output. substr(0, pos);
	}
}

//ACCESSORS - GETTERS//

std::string Response::getResponse( void ) const{
	return _response;
}

std::string		Response::getStatusLine(void) const{
	return _status_line;
}

std::string		Response::getHeader( void ) const{
	return _header;
}

std::string		Response::getBody( void ) const{
	return _body;
}

//ACCESSORS - SETTERS//
void			Response::setResponse( std::string resp ){
	_response = resp;
}

void			Response::setStatusLine( std::string status_l ){
	_status_line = status_l;
}

void			Response::setHeader( std::string header ){
	_header = header;
}

void			Response::setBody( std::string newbody ){
	_body = newbody;
}

}
