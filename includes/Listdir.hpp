#ifndef LISTDIR_HPP
# define LISTDIR_HPP

# include "webserv.hpp"

class listdir {
	public:
		listdir();
		~listdir();

		std::string					genetateAutoindex(std::string path, std::string loc);

	private:
		void						listFiles(std::string path, std::string loc);
		std::string					generateHTML( void );

		std::vector<std::string>	_dirs;
};

#endif