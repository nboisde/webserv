#include "Response.hpp"

namespace ws
{

Response::Response( void ) : _response("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 76\r\n\r\n<!DOCTYPE html><html><body><h1>First display on localhost</h1></body></html>\r\n\r\n")
{
};

Response::Response( Response const & src)
{
    *this = src;
}

Response::~Response() {};

Response &  Response::operator=( Response const & rhs)
{
    if ( this != &rhs )
        _response = rhs.getResponse();
    return *this;
}

std::string Response::getResponse() const
{
    return _response;
}

}