#ifndef FILE_HPP
# define FILE_HPP

#include "webserv.hpp"

namespace ws{
class Files
{
    public:
        Files( void );
        ~Files( void );

        void nextFile(void);
        void saveFilePiece(std::string raw, std::string bound);

        std::map<std::string, std::string> ups;
        int next;
};
}

#endif