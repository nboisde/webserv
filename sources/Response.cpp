#include "Response.hpp"

namespace ws
{

std::map<int, std::string>  Response::_status_code = {
    (100, "Continue"), 
    (200, "OK"),
    (300, "Multiple Choice"),
    (301, "Moved Permanently"),
    (400 "Bad Request"),
    (401 "Unauthorized"),
    (403 "Forbidden"),
    (404 "Not Found"),
    (500, "Internal Server Error")
}

Response::Response( void ) : _response("Hello from Server\n")
{

};

Response::Response( Response const & src)
{
    *this = src;
}

Response::~Response( void )
{

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

char*      Response::response( void ){
    //CONCATENER LES STRINGS POUR CREER LA REPONSE
    return response.c_str();
}

size_t      Response::response_size( void ){
    return  response.size();
}

//ACCESSORS - GETTERS//

std::string Response::getResponse( void ) const{
    return _response;
}

std::string		Response::getDate( void ) const{
    return _date;
}

std::string		Response::getStatusLine(void) const{
    return _status_line;
}

//ACCESSORS - SETTERS//
void			Response::setResponse( std::string resp ){
    _response = resp;
}

void			Response::setDate( std::string date ){
    _date = date;
}

void			Response::setStatusLine( std::string status_l ){
    _status_line = status_l;
}

}
