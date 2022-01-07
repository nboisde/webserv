#include "Response.hpp"

namespace ws
{

Response::Response( void ) : _response("Hello from Server\n")
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