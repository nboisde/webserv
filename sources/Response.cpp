#include "webserv.hpp"


namespace ws{

Response::Response() {};
Response::~Response() {};

std::string Response::getResponse() const
{
    return _response;
}
}