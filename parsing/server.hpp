#ifndef SERVER_CONF_HPP
# define SERVER_CONF_HPP

#include <cstring>

//https://www.plesk.com/blog/various/nginx-configuration-guide/

class route{
	private:
		bool get;
		bool post;
		bool del;
		
}

class server
{
	private:
		std::string server_name;
		int port;
		long body_size;
		std::string error_pages;
		std::vector<route> routes;
		bool server_valid;
};

#endif
