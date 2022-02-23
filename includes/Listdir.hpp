#ifndef LISTDIR_HPP
# define LISTDIR_HPP

# include "webserv.hpp"

class listdir {
	public:
		listdir();
		~listdir();

		std::string					generateAutoindex(std::string path, std::string loc);

	private:
		void						listFiles(std::string path, std::string loc);
		std::string					generateHTML( void );
		std::string					resolvePath( std::string path );


		std::vector<std::string>	_dirs;
};

#endif