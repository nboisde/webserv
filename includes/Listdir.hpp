#ifndef LISTDIR_HPP
# define LISTDIR_HPP

# include "webserv.hpp"

class listdir {
	public:
		listdir();
		~listdir();

		std::string					genetateAutoindex(std::string root, std::string path);

	private:
		void						listFiles(std::string root, std::string path);
		std::string					generateHTML( void );

		std::vector<std::string>	_dirs;
};

#endif