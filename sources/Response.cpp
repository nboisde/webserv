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

Response::Response( void ) : _response("Hello from Server\n"){
};

Response::Response( Response const & src){
    *this = src;
}

Response::~Response( void ){
};

Response &  Response::operator=( Response const & rhs)
{
    if ( this != &rhs )
        _response = rhs.getResponse();
        _status_line = rhs.getStatusLine();
        _date = rhs.getDate();
    return *this;
}

//METHODS - //

std::string 	Response::genStatusLine( void ){
	int ret_code = 200;
	std::stringstream ret;

	ret << "HTTP/1.1" << ret_code << _status_code[ret_code];	
	return ret.str();
}

std::string		getDate( void ){
}

std::string		Response::genHeader( void ){
}

const char *      Response::response( void ){
	_response.append(this->genStatusLine());

	const char * str = _response.c_str();
	return str;
}

size_t      Response::response_size( void ){
    return  _response.size();
}

//ACCESSORS - GETTERS//

std::string Response::getResponse( void ) const{
    return _response;
}

std::string		Response::getHeader( void ) const{
    return _header;
}

std::string		Response::getStatusLine(void) const{
    return _status_line;
}

//ACCESSORS - SETTERS//
void			Response::setResponse( std::string resp ){
    _response = resp;
}

void			Response::setHeader( std::string header ){
    _header = header;
}

void			Response::setStatusLine( std::string status_l ){
    _status_line = status_l;
}

}
