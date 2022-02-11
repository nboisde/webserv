#include "webserv.hpp"
#include "Files.hpp"

namespace ws{

Files::Files(): next(0){}
Files::~Files(){}

void Files::nextFile(void) {next++;}
void Files::saveFilePiece(std::string raw, std::string bound) {
    std::cout << YELLOW << raw << RESET << std::endl;
    std::cout << GREEN << bound << RESET << std::endl;
}
    

};