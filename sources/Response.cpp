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
		_content = rhs.getContent();
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
	
	_header += CRLF;
	return _header;
}

const char *      Response::response( void ){
	std::stringstream tmp;
	
	tmp << _status_line << CRLF;
	tmp << genHeader();

	tmp << _content;

	tmp << CRLF << CRLF;

	_response = tmp.str();
	
	std::cout << "RESPONSE = \n" << _response << std::endl;

	const char * str = _response.c_str();
	return str;
}

size_t      Response::response_size( void ){
	return  _response.size();
}

void		Response::treatCGI( std::string response )
{
	int pos;

	if ((pos = response.find("Status: ")) != -1)
	{
		pos += 8;
		_status_line = "HTTP/1.1 ";	
		_status_line +=  response.substr(pos, response.find("\r\n", pos) - pos);
		_content = response.substr(0, pos - 8);
		_content += response.substr(response.find("\r\n", pos) + 2);
	}
	else
		_content = response;
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

std::string		Response::getContent( void ) const{
	return _content;
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

void			Response::setContent( std::string newcontent ){
	_content = newcontent;
}

}
